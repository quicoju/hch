#ifndef ROOM_HH
#define ROOM_HH

#include <list>
#include "concepts.hh"

struct Room {
  Room(const char*, const std::list<Reservation>);

  /**
   * @brief Returns true if the room is available on a given date.
   *
   * @param Date to check if the reservation is available
   * @param [Duration] since Date to check for availability (default 1 day)
   */
  bool is_available_on(Date, Duration d=Days{1}) const;

  /**
   * @brief Reserve the room for the given Date and Duration
   *
   * The room will be locked for the specified Date and Duration.
   * This method will throw an exception if the reservation couldn't
   * be placed.
   *
   * @param Date that the reservation begins
   * @param [Duration] that the room is to be locked (default 1 day)
   */
  void reserve(Date, Duration d=Days{1});

  /* Properties */
  const std::string id;

private:
  std::list<Reservation> agenda;
};

#endif
