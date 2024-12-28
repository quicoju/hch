#include "Hotel.hh"

Hotel::Hotel(std::vector<Room> rooms)
  : rooms{rooms} {};

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms) const {
  for (const auto &r: rooms) {
    if (r.is_available_on(date, dur)) {
      --n_rooms;
      if (!n_rooms) return true;
    }
  }
  return false;
}
