#include "Room.hh"

#include <exception>

Room::Room(const char* id, const std::list<Reservation> r)
 : id{ id }
 , agenda{ r } {}

bool Room::is_available_on(Date date, Duration dur) const
{
  Period p{date, dur};

  for (const auto reservation: agenda)
    if (reservation.intersects(p)) return false;
  return true;
}

void  Room::reserve(Date date, Duration dur)
{
  if (!is_available_on(date, dur))
    throw std::logic_error{"Room is unavailable on the given Date/Duration"};

  agenda.push_back({date, dur});
}
