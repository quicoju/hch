#pragma once

#include <vector>

#include "Hotel.hh"

struct RoomData {
  std::string id;
  size_t capacity;
  Amenities amenities;
  Reservations reservations;
};

using RoomsData = std::vector<RoomData>;

struct HotelData {
  HotelData() : rooms{} {}
  HotelData(RoomsData r) : rooms{r} {}

  // Properties
  RoomsData rooms;
};
