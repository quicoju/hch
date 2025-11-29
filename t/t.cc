#include <chrono>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define APPROX(N) (Catch::Matchers::WithinAbs((N), 0.001))

#ifdef USE_sqlite
#include "t_sqlite.hh"
#else
#include "t_memory.hh"
#endif

using namespace std::chrono;

struct GlobalSetup {
    GlobalSetup() {
      std::cout << "Setting up tests...\n";
      prepare_tests();
    }
};
static GlobalSetup global_setup;

/* Hotel Tests
 * =========== */
#include "Hotel.hh"

TEST_CASE("Hotel", "[Hotel]") {
  auto src = build_src();
  Hotel hotel{&src};

  SECTION("is_available_on") {
    REQUIRE(hotel.rooms().size() == 8);
    Date date{2025, 01, 02};
    REQUIRE_FALSE(hotel.is_available_on(date, Days{2}, 30));
    REQUIRE(hotel.is_available_on(date, Days{2}, 2));
    REQUIRE(hotel.is_available_on(date));
  }
  SECTION("is_available_on (room)") {
    auto room = hotel.room("101");
    Date date{2024, 12, 23};
    REQUIRE(hotel.is_available_on(room, {2024, 12, 22}));
    REQUIRE(hotel.is_available_on(room, date));
    REQUIRE(hotel.is_available_on(room, date, Days{2}));
    REQUIRE(!hotel.is_available_on(room, {2024, 12, 19}));
    REQUIRE(!hotel.is_available_on(room, {2024, 12, 21}, Days{3}));
  }
  SECTION("find_available_on") {
    Date date{2024, 12, 19};
    SECTION("Case: available w/o amenity requirements") {
      auto available = hotel.find_available_on(date, Days{3});
      REQUIRE(available.size() == 6);
      REQUIRE(available[0].id == "103");
    }
    SECTION("Case: available with amenity requirements") {
      auto a = hotel.find_available_on(date, Days{3}, {Balcony});
      auto b = hotel.find_available_on(date, Days{3}, {Wifi, Balcony});
      REQUIRE(a.size() == 2);
      REQUIRE(a[0].id == "103");
      REQUIRE(a[1].id == "201");
      REQUIRE(b.size() == 1);
      REQUIRE(b.front().id == "103");
    }
    SECTION("Case: unavailable with amenities") {
      REQUIRE(hotel.find_available_on(date, Days{3}, {MiniBar, Wifi}).empty());
    }
  }

  SECTION("record_guest") {
    std::string guest_1{"juan.camaney@aol.com"};  // existing
    std::string guest_2{"john.bedney@yahoo.com"}; // non-existing
    REQUIRE(hotel.record_guest(guest_1) == guest_1);
    REQUIRE(hotel.record_guest(guest_2) == guest_2);
  }

  SECTION("reserve") {
    auto room = hotel.room("101");
    std::string guest{"juan.camaney@aol.com"};
    SECTION("Case: success") {
      REQUIRE(hotel.reserve(guest, "101", {2024,11,10}) == "W-0004");
      REQUIRE_FALSE(hotel.is_available_on(room, {2024,11,10}));

      REQUIRE(hotel.reserve(guest, "101", {2024,11,12}, Days{2}) == "W-0005");
      REQUIRE_FALSE(hotel.is_available_on(room, {2024,11,13}));
    }
    SECTION("Case: failed") {
      REQUIRE_THROWS_AS(hotel.reserve(guest, "101", {2024,12,20}), std::runtime_error);
    }
  }
  SECTION("cancel") {
    auto room = hotel.room("101");
    SECTION("Case: success") {
      hotel.cancel("A-001");
      REQUIRE(hotel.is_available_on(room, {2024,12,19}));
    }
    SECTION("Case: failed") {
      REQUIRE_THROWS_AS(hotel.cancel("A-010"), std::invalid_argument);
    }
  }
  SECTION("checkin") {
    auto utc_stamp  = DateTime{ seconds{1763152245} }; // 2025-11-14 20:30:45 UTC
    hotel.checkin("A-001", utc_stamp);

    auto r = hotel.room_reservations("101").front();
    REQUIRE(r.checkin_at == utc_stamp);
  }
  SECTION("checkout") {
    auto utc_stamp = DateTime{ seconds{1763238645} }; // 2025-11-15 20:30:45 UTC
    hotel.checkout("A-001", utc_stamp);

    auto r = hotel.room_reservations("101").front();
    REQUIRE(r.checkout_at == utc_stamp);
  }
  SECTION("find reservations") {
    SECTION("by ID") {
      auto r = hotel.reservation("A-001");
      REQUIRE(r.id == "A-001");
    }
    SECTION("by Room") {
      SECTION("Case: with reservations") {
        REQUIRE(hotel.room_reservations("101").size() == 1);
      }
      SECTION("Case: without reservations") {
        REQUIRE_FALSE(hotel.room_reservations("301").size());
      }
    }
    SECTION("by Guest") {
      SECTION("Case: with reservations") {
        REQUIRE(hotel.guest_reservations("juan.camaney@aol.com").size() == 3);
      }
      SECTION("Case: without reservations") {
        REQUIRE_FALSE(hotel.guest_reservations("me@gmail.com").size());
      }
    }
    SECTION("by starting date") {
      auto got = hotel.reservations_starting_on(Date{2024,12,31});
      REQUIRE(got.front().id == "A-003");
    }
    SECTION("by ending date") {
      auto got = hotel.reservations_ending_on({2025,01,04});
      REQUIRE(got.front().id == "A-003");
    }
  }
  SECTION("notes") {
    auto room = hotel.room("101");
    auto guest = "juan.camaney@aol.com";
    auto id = hotel.reserve(guest, "101", {2024,11,10});
    std::string default_note{
R"(Rates:
  date: 2024-11-10
  days: 1
  total: 105.99
  details:
    Base: 100.99
    Capacity: 0
    Wifi: 5)"
    };
    SECTION("reservation_notes") {
      REQUIRE(hotel.reservation_notes(id) == default_note);
    }
    SECTION("patch_reservation_notes") {
      hotel.patch_reservation_notes(id, "Title", "Content");
      REQUIRE(hotel.reservation_notes(id) == default_note + "\nTitle: Content");
      REQUIRE_THROWS_AS(
        hotel.patch_reservation_notes(id, "Rates", "new"),
        std::invalid_argument
      );
    }
  }
  SECTION("room") {
    SECTION("Case: existing") {
      REQUIRE(hotel.room("102").id == "102");
    }
    SECTION("Case: non-existing") {
      REQUIRE_THROWS_AS(hotel.room("A-102"), std::invalid_argument);
    }
  }
}


/* Room Tests
 * ========== */
#include "Room.hh"

TEST_CASE("Room class", "[Room]") {
  auto src = build_src();
  SECTION("construction") {
    auto room = Hotel{&src}.room("101");
    REQUIRE(room.id == "101");
    REQUIRE(room.capacity == 1);
    REQUIRE(room.amenities() == Amenities{Wifi});
  }
  SECTION("has_amenities") {
    auto room = Hotel{&src}.room("103");
    SECTION("Case: true") {
      REQUIRE(room.has_amenities({}));
      REQUIRE(room.has_amenities({Balcony}));
      REQUIRE(room.has_amenities({Balcony, Wifi}));
    }
    SECTION("Case: false") {
      REQUIRE_FALSE(room.has_amenities({MiniBar}));
    }
  }
  SECTION("find_by_id") {
    SECTION("Case: found") {
      auto room = Room::find_by_id("103", &src);
      REQUIRE(room.id == "103");
      REQUIRE(room.capacity == 3);
    }
    SECTION("Case: not found") {
      REQUIRE_THROWS_AS(Room::find_by_id("99", &src), std::invalid_argument);
    }
  }
  SECTION("find_all") {
    auto rooms = Room::find_all(&src);
    REQUIRE(rooms.size() == 8);
    REQUIRE(rooms.front().id == "101");
    REQUIRE(rooms.back().id == "302");
  }
}

/* RateCalculator Tests
 * ====================
 */
#include "RateCalculator.hh"

TEST_CASE("Rate::Calculator", "[Calculator]") {
  auto src = build_src();
  Hotel hotel{&src};
  Rate::Calculator calc(&src);
  auto standard_room = hotel.room("103");
  auto premium_room = hotel.room("101"); // with Wifi
  auto single_room = hotel.room("301");  // no amenities

  SECTION("base_rate_for") {
    REQUIRE_THAT(calc.base_rate_for(standard_room), APPROX(58.99));
    REQUIRE_THAT(calc.base_rate_for(premium_room), APPROX(100.99));
  }
  SECTION("capacity_rate_for") {
    REQUIRE_FALSE(calc.capacity_rate_for(single_room));
    REQUIRE_THAT(calc.capacity_rate_for(standard_room), APPROX(58.99*0.2*2));
  }
  SECTION("amenity_rate_for") {
    REQUIRE_THAT(calc.amenity_rate_for("Balcony"), APPROX(15.00));
    REQUIRE_FALSE(calc.amenity_rate_for("MiniBar"));
  }
  SECTION("rate_for") {
    SECTION("Case: standard room with default rate") {
      auto room = hotel.room("301");
      auto rate = calc.rate_for(room, {2024, 12, 23}, Days{1});
      REQUIRE_THAT(rate, APPROX(58.99)); // base rate only
    }
    SECTION("Case: premium room with specific rate") {
      auto rate = calc.rate_for(premium_room, {2024, 12, 23}, Days{1});
      REQUIRE_THAT(rate, APPROX(100.99 + 5.00));
    }
    SECTION("Case: single room w/o amenities multiple nights") {
      auto rate = calc.rate_for(single_room, {2024, 12, 23}, Days{3});
      auto expected = (58.99) * 3;
      REQUIRE_THAT(rate, APPROX(expected));
    }
    SECTION("Case: rooms with high capacity") {
      auto base_rate = 58.99;
      auto capacity_surcharge = base_rate * 0.20 * 2;
      SECTION("Case: standard rate") {
        auto big_room = hotel.room("302"); // capacity 3
        auto expected = base_rate + capacity_surcharge;
        auto rate = calc.rate_for(big_room, {2024, 12, 23}, Days{1});
        REQUIRE_THAT(rate, APPROX(expected));
      }
      SECTION("Case: standard rate with amenities") {
        auto big_room = hotel.room("103");
        auto expected = base_rate;
        expected += base_rate * 0.20 * 2; // additional capacity
        expected += 5.00 + 15.00;         // wifi + balcony
        expected *= 4;                    // 4 nights
        auto rate = calc.rate_for(big_room, {2024, 12, 23}, Days{4});
        REQUIRE_THAT(rate, APPROX(expected));
      }
    }
  }
}

/* Guest tests
 * ============
 */
#include "Guest.hh"

TEST_CASE("Guest") {
  auto src = build_src();
  std::string id{"juan.camaney@aol.com"};
  SECTION("Constructor") {
    REQUIRE(Guest{id, &src}.id == id);
  }
  SECTION("record") {
    SECTION("Existing") {
      REQUIRE(Guest::record(id, &src) == id);
    }
    SECTION("New") {
      std::string id2{"john.bedney@yahoo.com"};
      REQUIRE(Guest::record(id2, &src) == id2);
    }
  }
  SECTION("find_by_id") {
    REQUIRE(Guest::find_by_id(id, &src).id == id);
    REQUIRE_THROWS_AS(Guest::find_by_id("me@gmail.com", &src), std::runtime_error);
  }
}

/* Reservations tests
 * ==================
 */
#include "Reservation.hh"
TEST_CASE("Reservation") {
  auto src = build_src();
  SECTION("find_by_id") {
    SECTION("Case: found") {
      auto got = Reservation::find_by_id("A-001", &src);
      REQUIRE(got.id == "A-001");
      REQUIRE(got.guest_id == "juan.camaney@aol.com");
      REQUIRE(got.room_id == "101");
      REQUIRE(got.period == Period{{2024,12,19},Days{3}});
    }
    SECTION("Case: not found") {
      REQUIRE_THROWS_AS(Reservation::find_by_id("A-103", &src), std::invalid_argument);
    }
  }
  SECTION("find_by_room") {
    SECTION("Case: found") {
      auto got = Reservation::find_by_room("103", &src);
      REQUIRE(got.size() == 1);
      REQUIRE(got.back().id == "A-003");
    }
    SECTION("Case: not found") {
      REQUIRE_FALSE(Reservation::find_by_room("303", &src).size());
    }
  }
  SECTION("find_by_guest") {
    SECTION("Case: found") {
      auto got = Reservation::find_by_guest("juan.camaney@aol.com", &src);
      REQUIRE(got.size() == 3);
      REQUIRE(got.front().id == "A-001");
    }
    SECTION("Case: not found"){
      REQUIRE_FALSE(Reservation::find_by_guest("me@gmail.com", &src).size());
    }
  }
  SECTION("find_by_starting_date") {
    SECTION("Case: found") {
      auto got = Reservation::find_by_starting_date({2024,12,31}, &src);
      REQUIRE(got.front().id == "A-003");
    }
    SECTION("Case: not found") {
      REQUIRE(Reservation::find_by_starting_date({2025,12,12}, &src).empty());
    }
  }
  SECTION("find_by_ending_date") {
    SECTION("Case: found") {
      auto got = Reservation::find_by_ending_date({2025,01,04}, &src);
      REQUIRE(got.front().id == "A-003");
    }
    SECTION("Case: not found") {
      REQUIRE(Reservation::find_by_ending_date({2024,12,12}, &src).empty());
    }
  }
  SECTION("reserve") {
    std::string guest_id{ "juan.camaney@aol.com"};
    auto id = Reservation::reserve(guest_id, "101", {2024,12,12}, Days{5}, "", &src);
    // TODO: this test relies on an incremental counter
    // find a more determinitstic test for this
    REQUIRE(id == "W-0004");

    auto rsv = Reservation::find_by_id(id, &src);
    REQUIRE(rsv.room_id == "101");
    REQUIRE(rsv.period == Period{{2024,12,12}, Days{5}});
  }
  SECTION("cancel") {
    Reservation::find_by_id("A-001", &src).cancel();
    REQUIRE(Reservation::find_by_room("101", &src).size() == 0);
  }
  SECTION("checkin") {
    auto s = 1763152245;  // 2025-11-14 20:30:45 UTC
    auto utc_stamp = DateTime{ seconds{s} };
    Reservation::find_by_id("A-001", &src).checkin(utc_stamp);
    REQUIRE(Reservation::find_by_id("A-001", &src).checkin_at == utc_stamp);
  }
  SECTION("checkout") {
    auto s = 1763238645;  // 2025-11-15 20:30:45 UTC
    auto utc_stamp = DateTime{ seconds{s} };
    Reservation::find_by_id("A-001", &src).checkout(utc_stamp);
    REQUIRE(Reservation::find_by_id("A-001", &src).checkout_at == utc_stamp);
  }
  SECTION("annotate") {
    auto note = "My annotations";
    Reservation::find_by_id("A-001", &src).annotate(note);
    REQUIRE(Reservation::find_by_id("A-001", &src).notes == note);
  }
}
