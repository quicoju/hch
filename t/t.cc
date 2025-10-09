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
  auto src = build_agenda();
  Room room{ "101", 5, &src };
  REQUIRE(room.id == "101");
  REQUIRE(room.capacity == 5);
  REQUIRE(room.amenities() == Amenities{Wifi});
  REQUIRE(room.reservations() == Reservations{
      {{2024, 12, 19}, Days{3}},
      {{2024, 12, 25}, Days{5}},
    });
}

TEST_CASE("Room::is_available_on") {
  auto src = build_agenda();
  Room room{ "101", 1, &src };
  SECTION("available") {
    Date date{2024, 12, 23};
    REQUIRE(room.is_available_on({2024, 12, 22}));
    REQUIRE(room.is_available_on(date));
    REQUIRE(room.is_available_on(date, Days{2}));
    REQUIRE(room.is_available_on({2024, 12, 23}, Days{2}));
  }
  SECTION("unavailable") {
    REQUIRE(!room.is_available_on({2024, 12, 26}));
    REQUIRE(!room.is_available_on({2024, 12, 23}, Days{3}));
  }
}

TEST_CASE("Room::reserve") {
  auto src = build_agenda();
  Room room{ "101", 1, &src };
  SECTION("success") {
    Date date{2024, 11, 10};
    room.reserve(date);
    REQUIRE_FALSE(room.is_available_on(date));

    room.reserve({2024, 11, 12}, Days{2});
    REQUIRE_FALSE(room.is_available_on({2024, 11, 12}));
  }
  SECTION("failed") {
    REQUIRE_THROWS_AS(room.reserve({2024, 12, 20}), std::runtime_error);
  }
}

TEST_CASE("Room::cancel_reservation") {
  auto src = build_agenda();
  Room room{ "101", 1, &src };
  SECTION("success") {
    room.cancel_reservation({2024, 12, 20});
    REQUIRE(room.is_available_on({2024, 12, 21}));
  }
  SECTION("No reservation match") {
    room.cancel_reservation({2024, 12, 12});
    REQUIRE(!room.is_available_on({2024, 12, 25}));
  }
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
auto table_src = build_rate_table();
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
  auto src = build_agenda();

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
