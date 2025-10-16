#pragma once

#include <exception>
#include "concepts.hh"

struct Reservation {
  Reservation(const std::string& id, void* src);

  /**
   * @brief Find the reservations associted to a room
   *
   * @param room identifier to retrieve the reservations for
   * @param the source that holds the reservation information
   *
   * @return The recorded "Reservations" for the given room
   */
  static Reservations_
  find_by_room(const std::string& room_id, void* src);


  // state
  // =====
  std::string id;
  std::string guest_id;
  std::string room_id;
  Period period;

private:
  void* src;
};
