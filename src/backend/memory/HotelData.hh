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
  HotelData(std::string _) : rooms{} {
  // This constructor and data were made to feed
  // hch's src default hotel, it takes a string
  // as an argument to mirror the sqlite backend
  // constructor, this helps to define a "Backend"
  // type and make it very simple to build the backend
  // specific source without using generic programming
    using namespace Amenity;
    rooms = {
      {"101", 1, {Wifi}},
      {"102", 1, {Balcony, Wifi},},
      {"103", 1, {MiniBar, Wifi},},
      {"201", 1, {AirConditioning, Balcony},},
      {"202", 1, {Wifi},},
      {"203", 1, {MiniBar},}
    };
  }

  HotelData(RoomsData r) : rooms{r} {}

  // Properties
  RoomsData rooms;
};
