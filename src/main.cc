#include "Hotel.hh"

/*
 * TODO: For now write some unit tests here, maybe later
 * rename this fileo
 */
#include <catch2/catch_test_macros.hpp>

std::vector agenda = {
  Reservation{ Date{2024, 12, 19}, Days{3} },
  Reservation{ Date{2024, 12, 25}, Days{5} },
};

TEST_CASE("is_available_on") {
  Room room{ "101", agenda };
  SECTION("available") {
    Date date{2024, 12, 23};
    REQUIRE(room.is_available_on(date));
    REQUIRE(room.is_available_on(date, Days{2}));
    REQUIRE(room.is_available_on(Date{2024, 12, 23}, Days{2}));
  }
  SECTION("unavailable") {
    REQUIRE(!room.is_available_on(Date{2024, 12, 26}));
    REQUIRE(!room.is_available_on(Date{2024, 12, 23}, Days{3}));
  }
}

TEST_CASE("Hotel") {
  Hotel hotel {
    std::vector{
      Room{"101", std::vector{
          Reservation{ Date{2024, 12, 19}, Days{3} },
        }},
      Room{"102", std::vector{
          Reservation{ Date{2024, 12, 20}, Days{1} },
        }},
      Room{"103", std::vector{
          Reservation{ Date{2024, 12, 31}, Days{4} },
        }},
    }
  };
  REQUIRE(hotel.rooms.size() == 3);

  SECTION("is_available_on") {
    Date date{2025, 01, 02};
    REQUIRE_FALSE(hotel.is_available_on(date, Days{2}, 3));
    REQUIRE(hotel.is_available_on(date, Days{2}, 2));
    REQUIRE(hotel.is_available_on(date));
  }
}

