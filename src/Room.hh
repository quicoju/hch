#ifndef ROOM_HH
#define ROOM_HH

#include "concepts.hh"

struct Room {
  Room(const char*, const std::vector<Reservation>);

  /* Returns true if the room is available on the given
   * Date and Duration; otherwise returns false. */
  bool is_available_on(Date, Duration d=Days{1}) const;

  /* Properties */
  const std::string id;

private:
  const std::vector<Reservation> agenda;
};

#endif
