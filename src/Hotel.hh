#pragma once


#include "concepts.hh"
#include "Room.hh"

using RoomSet = std::vector<std::reference_wrapper<const Room>>;

struct Hotel {
  Hotel(std::vector<Room>);

  /* returns true if there are N_ROOMS available ot the given
   * DATE and the given DURATION, otherwise it returns false */
  bool is_available_on(Date, Duration dur=Days{1}, size_t n_rooms=1)
    const noexcept;

  RoomSet find_available_on(Date, Duration dur=Days{1})
    const noexcept;
  
  std::vector<Room> rooms;
};


