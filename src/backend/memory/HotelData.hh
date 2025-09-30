#pragma once

#include <vector>

#include "Hotel.hh"

struct RoomData {
  std::string id;
  size_t capacity;
  Amenities amenities;
  Reservations reservations;
};

using HotelData = std::vector<RoomData>;
