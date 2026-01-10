#pragma once

#include <chrono>

#include "HotelData.hh"

using namespace std::chrono;

inline auto conn_str(){ return ""; }
void prepare_tests(){}

HotelData build_src() {
  return HotelData{""};
}

HotelData build_src_with_reservations() {
  auto src = build_src();
  src.guests = { {"juan.camaney@aol.com"} };
  src.reservations = {
    {"A-001", "juan.camaney@aol.com", "101", 2024y/12/19d, 3},
    {"A-002", "juan.camaney@aol.com", "102", 2024y/12/20d, 1},
    {"A-003", "juan.camaney@aol.com", "103", 2024y/12/31d, 4},
  };
  return src;
}
