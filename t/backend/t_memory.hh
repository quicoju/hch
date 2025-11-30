#pragma once

#include "HotelData.hh"

void prepare_tests(){}

HotelData build_src() {
  return {
    { // rooms
      {"101", 1, {Wifi_}},
      {"102", 1, {Wifi_}},
      {"103", 3, {Balcony_, Wifi_}},
      {"201", 1, {Balcony_, AirConditioning_}},
      {"202", 1, {Wifi_}},
      {"203", 1, {MiniBar_}},
      {"301", 1, {}},
      {"302", 3, {}},
    },
    { // reservations
      {"A-001", "juan.camaney@aol.com", "101", {{2024,12,19}, Days(3)}},
      {"A-002", "juan.camaney@aol.com", "102", {{2024,12,20}, Days(1)}},
      {"A-003", "juan.camaney@aol.com", "103", {{2024,12,31}, Days{4}}},
    },
    { // rate table
      {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
      {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
      {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
      {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
      {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
    },
    { // gests
      {"juan.camaney@aol.com"}
    }
  };
}
