#include "Room.hh"

Room::Room(const char* id, const std::vector<Reservation> r)
 : id{ id }
 , agenda{ r } {}

bool Room::is_available_on(Date date, Duration dur) const {
  Period p{date, dur};

  for (const auto reservation: agenda)
    if (reservation.intersects(p)) return false;
  return true;
}
