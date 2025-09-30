#pragma once

#include <list>
#include <vector>

#include "Hotel.hh"

struct RoomData {
  std::string id;
  size_t capacity;
  std::list<Reservation> reservations;
};

using HotelData = std::vector<RoomData>;
