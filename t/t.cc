#include "Hotel.hh"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define APPROX(N) (Catch::Matchers::WithinAbs((N), 0.001))

#ifdef USE_sqlite
#include "t_sqlite.hh"
#else
#include "t_memory.hh"
#endif

struct GlobalSetup {
    GlobalSetup() {
      std::cout << "Setting up tests...\n";
      prepare_tests();
    }
};

static GlobalSetup global_setup;

/* Room Tests
 * ==========
 */
TEST_CASE("Room Initialization") {
  auto src = build_rooms();
  Room room{ "101", 5, &src };
  REQUIRE(room.id == "101");
  REQUIRE(room.capacity == 5);
  REQUIRE(room.amenities() == Amenities{Wifi});
}

TEST_CASE("Room::has_amenities") {
  auto src = room_with_amenities();
  Room room{"103", 3, &src};
  SECTION("true") {
    REQUIRE(room.has_amenities({}));
    REQUIRE(room.has_amenities({Balcony}));
    REQUIRE(room.has_amenities({Balcony, Wifi}));
  }
  SECTION("false") {
    REQUIRE_FALSE(room.has_amenities({MiniBar}));
  }
}

/* Hotel Tests
 * ===========
 */
TEST_CASE("Hotel::is_available_on") {
  auto src = build_src();
  Hotel hotel{&src};
  SECTION("Hotel::is_available_on") {
    REQUIRE(hotel.rooms().size() == 3);
    Date date{2025, 01, 02};
    REQUIRE_FALSE(hotel.is_available_on(date, Days{2}, 3));
    REQUIRE(hotel.is_available_on(date, Days{2}, 2));
    REQUIRE(hotel.is_available_on(date));
  }

  SECTION("Hotel::is_available_on (room)") {
    Room room{"101", 1, &src}; // TODO: fix this source
    Date date{2024, 12, 23};
    REQUIRE(hotel.is_available_on(room, {2024, 12, 22}));
    REQUIRE(hotel.is_available_on(room, date));
    REQUIRE(hotel.is_available_on(room, date, Days{2}));
    REQUIRE(hotel.is_available_on(room, {2024, 12, 23}, Days{2}));
    REQUIRE(!hotel.is_available_on(room, {2024, 12, 19}));
    REQUIRE(!hotel.is_available_on(room, {2024, 12, 21}, Days{3}));
  }
}

TEST_CASE("Hotel::find_available_on") {
  auto src = build_src();
  Hotel hotel{&src};
  SECTION("available w/o amenities") {
    Date date{2024, 12, 19};
    auto available = hotel.find_available_on(date, Days{3});
    REQUIRE(available.size() == 1);
    REQUIRE(available[0].id == "103");
  }

  SECTION("available with amenities") {
    Date date{2024, 12, 19};
    auto a = hotel.find_available_on(date, Days{3}, {Balcony});
    auto b = hotel.find_available_on(date, Days{3}, {Wifi, Balcony});
    REQUIRE(a.size() == 1);
    REQUIRE(a[0].id == "103");
  }

  SECTION("unavailable with amenities") {
    Date date{2024, 12, 19};
    REQUIRE(hotel.find_available_on(date, Days{3}, {MiniBar}).empty());
  }

  SECTION("unavailable w/o amenities") {
    Date date{2024, 12, 1};
    REQUIRE(hotel.find_available_on(date, Days{40}).empty());
  }
}

TEST_CASE("Hotel::reserve") {
  auto src = build_src();
  Hotel hotel{&src};
  Room room{"101", 1, &src}; // TODO: fix this source
  std::string guest{"juan.camaney@aol.com"};

  SECTION("success") {
    Date date{2024,11,10};
    hotel.reserve(guest, room, date);
    REQUIRE_FALSE(hotel.is_available_on(room, date));

    hotel.reserve(guest, room, {2024,11,12}, Days{2});
    REQUIRE_FALSE(hotel.is_available_on(room, {2024,11,13}));
  }
  SECTION("failed") {
    REQUIRE_THROWS_AS(hotel.reserve(guest, room, {2024,12,20}), std::runtime_error);
  }
}

TEST_CASE("Hotel::cancel") {
  auto src = build_src();
  Hotel hotel{&src};
  Room room{"101", 1, &src}; // TODO: fix this source
  SECTION("success") {
    hotel.cancel("A-001");
    hotel.is_available_on(room, {2024,12,19});
  }
  SECTION("failed") {
    REQUIRE_THROWS_AS(hotel.cancel("A-010"), std::invalid_argument);
  }
}

TEST_CASE("Hotel::reservations_for") {
  auto src = build_src();
  Hotel hotel{&src};
  Room room{"101", 1, &src};
  SECTION("With reservations") {
    REQUIRE(hotel.reservations_for(room).size() == 1);
  }
  // TODO: test the case without reservations
}

TEST_CASE("Hotel::room") {
  auto src = build_one_room_src();
  Hotel hotel{&src};
  SECTION("existing") {
    REQUIRE(hotel.room("A-102").id == "A-102");
  }
  SECTION("non-existing") {
    REQUIRE_THROWS_AS(hotel.room("102"), std::invalid_argument);
  }
}

/* RateCalculator Tests
 * ====================
 */
#include "RateCalculator.hh"

auto table_src = build_src();
TEST_CASE("basic rate accessors") {
  Rate::Calculator calc(&table_src);
  auto src_1 = room_with_amenities(); // capacity 3, Balcony, Wifi
  Room standard_room{"103", 3, &src_1};
  Room premium_room{"101", 3, &src_1};

  auto src_2 = room_without_wifi(); // capacity 1
  Room single_room{"301", 1, &src_2};

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
}

TEST_CASE("Rate::Calculator::rate_for - Basic room pricing") {
  Rate::Calculator calc(&table_src);
  auto src = room_without_wifi();

  SECTION("Standard room with default rate") {
    Room room{"102", 1, &src}; // capacity 1, basic room
    auto rate = calc.rate_for(room, {2024, 12, 23}, Days{1});
    REQUIRE_THAT(rate, APPROX(58.99)); // base rate only
  }

  SECTION("Premium room with specific rate") {
    Room room{"101", 1, &src}; // room 101 has premium rate
    auto rate = calc.rate_for(room, {2024, 12, 23}, Days{1});
    REQUIRE_THAT(rate, APPROX(100.99)); // premium base rate
  }

  SECTION("Multiple nights") {
    Room room{"102", 1, &src};
    auto rate = calc.rate_for(room, {2024, 12, 23}, Days{3});
    auto expected = 58.99 * 3;
    REQUIRE_THAT(rate, APPROX(expected));
  }
}

TEST_CASE("Rate::Calculator::rate_for - Capacity pricing") {
  Rate::Calculator calc(&table_src);
  auto src = room_without_wifi();

  SECTION("Higher capacity room") {
    Room room{"102", 3, &src}; // capacity 3
    auto base_rate = 58.99;
    auto capacity_surcharge = base_rate * 0.20 * 2;
    auto expected = base_rate + capacity_surcharge;
    auto rate = calc.rate_for(room, {2024, 12, 23}, Days{1});
    REQUIRE_THAT(rate, APPROX(expected));
  }
}

TEST_CASE("Rate::Calculator::rate_for - Amenity pricing") {
  Rate::Calculator calc{&table_src};
  auto src = build_rooms();

  SECTION("Room with Wifi amenity") {
    Room room{"101", 1, &src};
    auto rate = calc.rate_for(room, {2024, 12, 23}, Days{1});
    auto expected = 100.99 + 5.00; // premium rate + wifi
    REQUIRE_THAT(rate, APPROX(expected));
  }
}

TEST_CASE("Rate::Calculator::rate_for - Complex pricing") {
  Rate::Calculator calc{&table_src};
  auto src = room_with_amenities();

  SECTION("High capacity room with multiple amenities") {
    Room room{"103", 3, &src};
    auto base_rate = 58.99;
    auto expected = base_rate;
    expected += base_rate * 0.20 * 2; // additional capacity
    expected += 5.00 + 15.00;         // wifi + balcony
    expected *= 4;                    // 4 nights

    auto rate = calc.rate_for(room, {2024, 12, 23}, Days{4});
    REQUIRE_THAT(rate, APPROX(expected));
  }
}


/* Guest tests
 * ============
 */
#include "Guest.hh"

TEST_CASE("Guest") {
  auto guest = "juan.camaney@aol.com";
  auto src = some_guests({guest});
  SECTION("Existing Guest") {
    REQUIRE(Guest{guest, &src}.id() == guest);
  }
  // SECTION("Non-existing Guest") {
  //   REQUIRE_THROWS_AS(Guest("me@gmail.com", &src), std::runtime_error);
  // }
}

/* Reservations tests
 * ==================
 */
#include "Reservation.hh"
TEST_CASE("Reservation") {
  auto src = build_src();
  SECTION("find_by_id") {
    auto got = Reservation::find_by_id("A-001", &src);
    REQUIRE(got.id == "A-001");
    REQUIRE(got.guest_id == "juan.camaney@aol.com");
    REQUIRE(got.room_id == "101");
    REQUIRE(got.period == Period{{2024,12,19},Days{3}});
    REQUIRE_THROWS_AS(Reservation::find_by_id("A-103", &src), std::invalid_argument);
  }
  SECTION("find_by_room") {
    auto got = Reservation::find_by_room("103", &src);
    REQUIRE(got.size() == 1);
    REQUIRE(got.back().id == "A-003");
  }
  SECTION("reserve") {
    auto id = Reservation::reserve(
      "juan.camaney@aol.com", "101", {2024,12,12}, Days{5}, &src);
    auto rsv = Reservation::find_by_id(id, &src);

    // TODO: preivous tests add reservations so the counter increases
    // find a more determinitstic test for this
    REQUIRE(rsv.id == "W-0004");
    REQUIRE(rsv.room_id == "101");
    REQUIRE(rsv.period == Period{{2024,12,12}, Days{5}});
  }
  SECTION("cancel") {
    Reservation::find_by_id("A-001", &src).cancel();
    REQUIRE(Reservation::find_by_room("101", &src).size() == 0);
  }
}
