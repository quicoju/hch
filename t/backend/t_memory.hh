#pragma once

#include "HotelData.hh"

void prepare_tests(){}

RoomData build_agenda() {
  return {"101", 1, {Wifi}, {
      { {2024, 12, 19}, Days{3} },
      { {2024, 12, 25}, Days{5} },
    }};
}

RoomData room_without_wifi() {
  return {"301", 1, {}, {}};
}

RoomData room_with_amenities() {
  return {"103", 3, {Balcony, Wifi}, {}};
}

HotelData build_one_room_src() {
  return {{
    {"A-102", 1, {Wifi}, },
    }};
}

HotelData build_src() {
  return {{
    {"101", 1, {Wifi}, {{{2024, 12, 19}, Days{3} }}},
    {"102", 1, {Wifi}, {{{2024, 12, 20}, Days{1} }}},
    {"103", 3, {Balcony, Wifi}, {{{2024, 12, 31}, Days{4} }}},
    }};
}

// Rate tests
Rate::Table build_rate_table() {
  return Rate::Table {
    {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
    {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
    {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
    {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
    {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
  };
}
