#pragma once

#include "concepts.hh"
#include "Room.hh"

struct Hotel {
  Hotel(void *src);

  /**
   * @brief Returns true only if there are N_ROOMS available on
   * in the hotel for the given DATE and DURATION,
   *
   * @param Check for availability on this number of rooms
   * @param Check for availability starting on this date
   * @param Number of days that the rooms are required to be available
   */
  bool is_available_on(Date, Duration dur=Days{1}, size_t n_rooms=1)
    const noexcept;

  /**
   * @brief Return all the available rooms for the given DATE and
   *  DURATION,
   *
   * @param Check for availability starting on this date
   * @param Number of days that the rooms are required to be available
   */
  Rooms find_available_on(Date, Duration dur=Days{1})
    const noexcept;

  /**
   * @brief Returns the room identified with ID
   *
   * @param The room identifier
   *
   */
  Room room(const std::string id);

  /**
   * @brief Return all the rooms
   *
   */
  Rooms rooms() const;

private:
  void *src;
};

