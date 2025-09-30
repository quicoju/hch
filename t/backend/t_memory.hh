#pragma once

#include "../src/backend/memory/HotelData.hh"

using enum Amenity;

RoomData build_agenda() {
  return {"101", 1, {Wifi}, {
      { {2024, 12, 19}, Days{3} },
      { {2024, 12, 25}, Days{5} },
    }};
}

HotelData build_one_room_src() {
  return {
    {"A-102", 1, {Wifi}, },
  };
}

HotelData build_src() {
  return {
    {"101", 1, {Wifi}, {{{2024, 12, 19}, Days{3} }}},
    {"102", 1, {Wifi}, {{{2024, 12, 20}, Days{1} }}},
    {"103", 1, {Wifi}, {{{2024, 12, 31}, Days{4} }}},
  };
}
