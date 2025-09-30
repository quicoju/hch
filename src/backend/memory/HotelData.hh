#pragma once

#include <vector>

#include "Hotel.hh"

struct RoomData {
  std::string id;
  size_t capacity;
  Reservations reservations;
};

using HotelData = std::vector<RoomData>;
