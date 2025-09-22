#include "../src/Hotel.hh"

#include <catch2/catch_test_macros.hpp>

std::list agenda{
  Reservation{ {2024, 12, 19}, Days{3} },
  Reservation{ {2024, 12, 25}, Days{5} },
};

/* Room Tests
 * ==========
 */
TEST_CASE("Room::is_available_on") {
  auto a = agenda;
  Room room{ "101", &a };
  SECTION("available") {
    Date date{2024, 12, 23};
    REQUIRE(room.is_available_on({2024, 12, 22}));
    REQUIRE(room.is_available_on(date));
    REQUIRE(room.is_available_on(date, Days{2}));
    REQUIRE(room.is_available_on({2024, 12, 23}, Days{2}));
    std::cout << str(a.front()) << std::endl;
  }
  SECTION("unavailable") {
    REQUIRE(!room.is_available_on({2024, 12, 26}));
    REQUIRE(!room.is_available_on({2024, 12, 23}, Days{3}));
  }
}

TEST_CASE("Room::reserve") {
  auto a = agenda;
  Room room{ "101", &a };
  SECTION("success") {
    Date date{2024, 11, 10};
    room.reserve(date);
    REQUIRE_FALSE(room.is_available_on(date));

    room.reserve({2024, 11, 12}, Days{2});
    REQUIRE_FALSE(room.is_available_on({2024, 11, 12}));
  }
  SECTION("failed") {
    REQUIRE_THROWS_AS(room.reserve({2024, 12, 20}), std::logic_error);
  }
}

TEST_CASE("Room::cancel_reservation") {
  auto a = agenda;
  Room room{ "101", &a };
  SECTION("success") {
    room.cancel_reservation({2024, 12, 20});
    REQUIRE(room.is_available_on({2024, 12, 21}));
  }
  SECTION("No reservation match") {
    room.cancel_reservation({2024, 12, 12});
    REQUIRE(!room.is_available_on({2024, 12, 25}));
  }
}

TEST_CASE("Room::reservations") {
  auto a = agenda;
  Room room{ "101", &a };
  REQUIRE(room.reservations() == agenda);
}

/* Hotel Tests
 * ===========
 */
using Reservations = std::list<Reservation>;

TEST_CASE("Hotel::is_available_on") {
  Reservations r_101{ {{2024, 12, 19}, Days{3} }};
  Reservations r_102{ {{2024, 12, 20}, Days{1} }};
  Reservations r_103{ {{2024, 12, 31}, Days{4} }};
  Rooms rooms{
    {"101", &r_101},
    {"102", &r_102},
    {"103", &r_103},
  };
  Hotel hotel{&rooms};

  REQUIRE(hotel.rooms().size() == 3);

  SECTION("Hotel::is_available_on") {
    Date date{2025, 01, 02};
    REQUIRE_FALSE(hotel.is_available_on(date, Days{2}, 3));
    REQUIRE(hotel.is_available_on(date, Days{2}, 2));
    REQUIRE(hotel.is_available_on(date));
  }
}

TEST_CASE("Hotel::find_available_on") {
  Reservations r_101{ {{2024, 12, 19}, Days{3} }};
  Reservations r_102{ {{2024, 12, 20}, Days{1} }};
  Reservations r_103{ {{2024, 12, 31}, Days{4} }};
  Rooms rooms{
    {"101", &r_101},
    {"102", &r_102},
    {"103", &r_103},
  };
  Hotel hotel{&rooms};
  
  SECTION("available") {
    Date date{2024, 12, 19};
    auto available = hotel.find_available_on(date, Days{3});
    REQUIRE(available.size() == 1);
    REQUIRE(available[0].id == "103");
  }

  SECTION("unavailable") {
    Date date{2024, 12, 1};
    REQUIRE(hotel.find_available_on(date, Days{40}).empty());
  }
}

TEST_CASE("Hotel::room") {
  Reservations r{};
  Rooms rooms{{"A-102", &r}};
  Hotel hotel{&rooms};
  SECTION("existing") {
    REQUIRE(hotel.room("A-102").id == "A-102");
  }
  SECTION("non-existing") {
    REQUIRE_THROWS_AS(hotel.room("102"), std::invalid_argument);
  }
}
