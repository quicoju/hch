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

using namespace Amenity;
struct HotelData {
  HotelData(std::string _) : rooms{} {
    rooms = {
      {"101", 1, {Wifi}},
      {"102", 1, {Balcony, Wifi}, },
      {"103", 1, {MiniBar, Wifi}, },
      {"201", 1, {AirConditioning, Balcony}, },
      {"202", 1, {Wifi}, },
      {"203", 1, {MiniBar}, },
    };
  }
  HotelData(RoomsData r) : rooms{r} {}

  // Properties
  RoomsData rooms;
};
