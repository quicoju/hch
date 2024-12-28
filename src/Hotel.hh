#ifndef HOTEL_HH
#define HOTEL_HH

#include "Room.hh"

struct Hotel {
  Hotel(std::vector<Room>);

  /* returns true if there are N_ROOMS available ot the given
   * DATE and the given DURATION, otherwise it returns false */
  bool is_available_on(Date, Duration dur=Duration{1}, size_t n_rooms=1) const;
  
  std::vector<Room> rooms;
};

#endif
