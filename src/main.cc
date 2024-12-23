#include <vector>
#include <utility>
#include <boost/date_time/gregorian/gregorian.hpp>

using Date = boost::gregorian::date;
using Duration = boost::gregorian::date_duration;
using Reservation = boost::gregorian::date_period;
using Period = boost::gregorian::date_period;

struct Room {
  Room(const char* id, const std::vector<Reservation> r)
    : id{ id }
    , agenda{ r } {}
  
  bool is_available_on(Date date, Duration dur=Duration{1}) const {
    /* Returns true if the room is available on the given
     * DATE, otherwise returns false. */
    Period p{date, dur};
    
    for (const auto reservation: agenda)
      if (reservation.intersects(p)) return false;
    return true;
  }

  const char* id;
  const std::vector<Reservation> agenda;
};

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
