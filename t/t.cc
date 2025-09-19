#include "../src/Hotel.hh"

#include <catch2/catch_test_macros.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

std::list agenda{
  Reservation{ {2024, 12, 19}, Days{3} },
  Reservation{ {2024, 12, 25}, Days{5} },
};

/* Room Tests
 * ==========
 */
TEST_CASE("Room::is_available_on") {
  Room room{ "101", agenda };
  SECTION("available") {
    Date date{2024, 12, 23};
    REQUIRE(room.is_available_on({2024, 12, 22}));
    REQUIRE(room.is_available_on(date));
    REQUIRE(room.is_available_on(date, Days{2}));
    REQUIRE(room.is_available_on({2024, 12, 23}, Days{2}));
    std::cout << boost::gregorian::to_simple_string(agenda.front()) << std::endl;
  }
  SECTION("unavailable") {
    REQUIRE(!room.is_available_on({2024, 12, 26}));
    REQUIRE(!room.is_available_on({2024, 12, 23}, Days{3}));
  }
}

TEST_CASE("Room::reserve") {
  Room room{ "101", agenda };
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
  Room room{ "101", agenda };
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
  Room room{ "101", agenda };
  REQUIRE(room.reservations() == agenda);
}

/* Hotel Tests
 * ===========
 */

TEST_CASE("Hotel::is_available_on") {
  Hotel hotel{{
      Room{"101", {
          Reservation{ {2024, 12, 19}, Days{3} },
        }},
      Room{"102", {
          Reservation{ {2024, 12, 20}, Days{1} },
        }},
      Room{"103", {
          Reservation{ {2024, 12, 31}, Days{4} },
        }},
    }};
  REQUIRE(hotel.rooms.size() == 3);

  SECTION("Hotel::is_available_on") {
    Date date{2025, 01, 02};
    REQUIRE_FALSE(hotel.is_available_on(date, Days{2}, 3));
    REQUIRE(hotel.is_available_on(date, Days{2}, 2));
    REQUIRE(hotel.is_available_on(date));
  }
}

TEST_CASE("Hotel::find_available_on") {
  Hotel hotel {{
      Room{"101", {
          Reservation{ {2024, 12, 19}, Days{3} },
        }},
      Room{"102", {
          Reservation{ {2024, 12, 20}, Days{1} },
        }},
      Room{"103", {
          Reservation{ {2024, 12, 31}, Days{4} },
        }},
    }};
  
  SECTION("available") {
    Date date{2024, 12, 19};
    auto available = hotel.find_available_on(date, Days{3});
    REQUIRE(available.size() == 1);
    REQUIRE(available.front().get().id == "103");
  }

  SECTION("unavailable") {
    Date date{2024, 12, 1};
    REQUIRE(hotel.find_available_on(date, Days{40}).empty());
  }
}

TEST_CASE("Hotel::room") {
  Hotel hotel {{
      Room{"A-102", {}},
    }};

  SECTION("existing") {
    REQUIRE(hotel.room("A-102").id == "A-102");
  }
  SECTION("non-existing") {
    REQUIRE_THROWS_AS(hotel.room("102"), std::invalid_argument);
  }
}
