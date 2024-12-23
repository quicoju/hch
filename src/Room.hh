#ifndef ROOM_HH
#define ROOM_HH

#include <boost/date_time/gregorian/gregorian.hpp>

using Date = boost::gregorian::date;
using Duration = boost::gregorian::date_duration;
using Reservation = boost::gregorian::date_period;
using Period = boost::gregorian::date_period;

struct Room {
  Room(const char*, const std::vector<Reservation>);

  /* Returns true if the room is available on the given
   * Date and Duration; otherwise returns false. */
  bool is_available_on(Date, Duration d=Duration{1}) const;

  /* Properties */
  const char* id;

private:
  const std::vector<Reservation> agenda;
};

#endif
