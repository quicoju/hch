#include "Hotel.hh"

Hotel::Hotel(std::vector<Room> rooms)
  : rooms{rooms} {};

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const noexcept {
  for (const auto &r: rooms) {
    if (r.is_available_on(date, dur)) {
      --n_rooms;
      if (!n_rooms) return true;
    }
  }
  return false;
}

MatchingRooms Hotel::find_available_on(Date d, Duration dur)
  const noexcept {
  MatchingRooms available_rooms;

  for (const auto &r: rooms) {
    if (r.is_available_on(d, dur))
      available_rooms.push_back(std::cref(r));
  }
  return available_rooms;
}
