#pragma once

#include "HotelData.hh"

void prepare_tests(){}

RoomData build_rooms() {
  return {"101", 1, {Wifi}};
}

RoomData room_without_wifi() {
  return {"301", 1, {}};
}

RoomData room_with_amenities() {
  return {"103", 3, {Balcony, Wifi}};
}

HotelData build_one_room_src() {
  return {{
      {"A-102", 1, {Wifi}},
    }};
}

HotelData build_src() {
  return {
    { // rooms
      {"101", 1, {Wifi}},
      {"102", 1, {Wifi}},
      {"103", 3, {Balcony, Wifi}},
    },
    { // reservations
      {"A-001", "juan.camaney@aol.com", "101", {{2024,12,19}, Days(3)}},
      {"A-002", "juan.camaney@aol.com", "102", {{2024,12,20}, Days(1)}},
      {"A-003", "juan.camaney@aol.com", "103", {{2024,12,31}, Days{4}}},
    }
  };
}

// Guest tests
// ===========
GuestsData some_guests(const std::vector<std::string>& ids)
{
  GuestsData guests{};
  for (const auto& id: ids)
    guests.emplace_back(id);
  return guests;
}

// Rate tests
// ===========
Rate::Table build_rate_table() {
  return Rate::Table {
    {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
    {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
    {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
    {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
    {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
  };
}

// Reservation tests
// =================
ReservationsData some_reservations()
{
  return {
    {"A-001", "juan.camaney@aol.com", "101", {{2024,11,01}, Days(2)}},
    {"A-002", "juan.camaney@aol.com", "201", {{2024,12,18}, Days(1)}},
    {"A-003", "juan.camaney@aol.com", "101", {{2025,01,01}, Days{5}}},
  };
}
