#include "Room.hh"

/*
 * TODO: For now write some unit tests here, maybe later
 * rename this fileo
 */
#include<catch2/catch_test_macros.hpp>

std::vector agenda = {
  Reservation{ Date{2024, 12, 19}, Duration{3} },
  Reservation{ Date{2024, 12, 25}, Duration{5} },
};

TEST_CASE("is_available_on") {
  Room room{ "101", agenda };
  SECTION("available") {
    Date date{2024, 12, 23};
    REQUIRE(room.is_available_on(date));
    REQUIRE(room.is_available_on(date, Duration{2}));
    REQUIRE(room.is_available_on(Date{2024, 12, 23}, Duration{2}));
  }
  SECTION("unavailable") {
    REQUIRE(!room.is_available_on(Date{2024, 12, 26}));
    REQUIRE(!room.is_available_on(Date{2024, 12, 23}, Duration{3}));
  }
}
