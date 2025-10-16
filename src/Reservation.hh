#pragma once

#include <exception>
#include "concepts.hh"

struct Reservation {
  Reservation(const std::string& id, void* src);

private:
  void* src;
  std::string id;
  std::string guest_id;
  std::string room_id;
  Period period;
};
