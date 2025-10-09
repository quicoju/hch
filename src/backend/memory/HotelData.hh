#pragma once

#include <vector>

#include "Hotel.hh"

/* Rate type definitions
 * =====================
 * See the memory/RateCalculator.cc for a description of how
 * the rate types are used.
 */
namespace Rate {
  enum class Type {
    Base,
    Capacity,
    Amenity
  };

  struct Entry {
    Type type;
    std::string key;
    double value;
  };

  using Table = std::vector<Entry>;
}

struct RoomData {
  std::string id;
  size_t capacity;
  Amenities amenities;
  Reservations reservations;
};

using RoomsData = std::vector<RoomData>;

struct HotelData {
  HotelData(std::string _) : rooms{}, rates{} {
  // This constructor and data were made to feed
  // hch's src default hotel, it takes a string
  // as an argument to mirror the sqlite backend
  // constructor, this helps to define a "Backend"
  // type and make it very simple to build the backend
  // specific source without using generic programming
    rooms = {
      {"101", 1, {Wifi}},
      {"102", 1, {Balcony, Wifi},},
      {"103", 1, {MiniBar, Wifi},},
      {"201", 1, {AirConditioning, Balcony},},
      {"202", 1, {Wifi},},
      {"203", 1, {MiniBar},}
    };
    rates = {
      {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
      {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
      {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
      {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
      {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
    };
  }

  HotelData(RoomsData r) : rooms{r}, rates{} {}

  // Properties
  RoomsData rooms;
  Rate::Table rates;
};
