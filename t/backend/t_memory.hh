#pragma once

#include "HotelData.hh"

inline auto conn_str(){ return ""; }
void prepare_tests(){}

HotelData build_src() {
  return HotelData{""};
}

HotelData build_src_with_reservations() {
  auto src = build_src();
  src.guests = { {"juan.camaney@aol.com"} };
  src.reservations = {
    {"A-001", "juan.camaney@aol.com", "101", {{2024,12,19}, Days(3)}},
    {"A-002", "juan.camaney@aol.com", "102", {{2024,12,20}, Days(1)}},
    {"A-003", "juan.camaney@aol.com", "103", {{2024,12,31}, Days{4}}},
  };
  return src;
}
