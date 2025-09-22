#pragma once


#include "concepts.hh"
#include "Room.hh"

using Rooms = std::vector<Room>;

struct Hotel {
  Hotel(void *);

  /* returns true if there are N_ROOMS available ot the given
   * DATE and the given DURATION, otherwise it returns false */
  bool is_available_on(Date, Duration dur=Days{1}, size_t n_rooms=1)
    const noexcept;

  Rooms find_available_on(Date, Duration dur=Days{1})
    const noexcept;

  Room &room(const std::string id);

  Rooms &rooms() const;

private:
  void *src;
};


