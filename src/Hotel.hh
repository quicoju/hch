#pragma once

#include "concepts.hh"
#include "Room.hh"
#include "RateCalculator.hh"

struct Hotel {
  Hotel(void* src) noexcept : src{src} {};

  /**
   * @brief Returns true only if there are N_ROOMS available on
   * in the hotel for the given DATE and DURATION,
   *
   * @param Check for availability on this number of rooms
   * @param Check for availability starting on this date
   * @param Number of days that the rooms are required to be available
   */
  bool is_available_on(Date, Duration dur=Days{1}, size_t n_rooms=1)
    const;

  /**
   * @brief Check if a ROOM is available on during the given DATE and DURATION
   *
   * @param The room to check availability for
   * @param Date to check for availability (defaults to Today)
   * @param The duration in days to check for availability (defaults to one)
   *
   * @return true if is available, false otherwise
   */
  bool is_available_on(Room, Date date=Today, Duration dur=Days{1})
    const;

  /**
   * @brief Return all the available rooms for the given DATE and
   *  DURATION,
   *
   * @param Check for availability starting on this date
   * @param Number of days that the rooms are required to be available
   */
  Rooms find_available_on(Date, Duration dur=Days{1}, Amenities amenities={})
    const;

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

  /**
   * @brief provide rate report associated to the room
   *
   * @param Room identifier
   * @param Date for the rate to be calculated (defaults to Today)
   * @param Number of days for the rate to be calculated (defaults to 1)
   */
  const RateReport
  rate_report_for(Room, Date date=Today, Duration dur=Days{1}) const;

private:
  void* src;
};
