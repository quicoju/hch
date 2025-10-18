#pragma once

#include <exception>
#include "concepts.hh"

struct Reservation {
  Reservation(const std::string& id,
              const std::string& guest_id,
              const std::string& room_id,
              const Period& period,
              void* src)
    : id{id}
    , guest_id{guest_id}
    , room_id{room_id}
    , period{period}
    , src{src}
  {}

  /** @brief Make a new reservation
   *
   *  @param guest identifier
   *  @param room identifier
   *  @param date where the room is to be reserved (defaults to Today)
   *  @param duration in days that the room will reserved (defaults to one)
   *
   *  @return a unique identifier of the reservation
   */
  static std::string
  reserve(const std::string& guest_id,
          const std::string& room_id,
          Date date,
          Duration duration,
          void* src);


  /** @brief Cancel a reservation
   *
   */
  void cancel();

  /**
   * @brief Find a reservation using its ID
   *
   * This method will throw an "invalid_argument" if the
   * reservation doesn't exist
   *
   * @param reservation identifier
   * @param the source that holds the reservation information
   *
   * @return The matched "Reservation"
   */
  static Reservation
  find_by_id(const std::string& id, void* src);

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
