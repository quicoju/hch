#pragma once

#include <exception>
#include "concepts.hh"

struct Reservation {
  Reservation(const std::string& id,
              const std::string& guest_id,
              const std::string& room_id,
              const Period& period,
              const std::string& notes,
              void* src)
    : id{id}
    , guest_id{guest_id}
    , room_id{room_id}
    , period{period}
    , notes{std::move(notes)}
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
          std::string_view notes,
          void* src);


  /**
   * @brief Cancel a reservation
   *
   */
  void cancel();

  /**
   * @brief Store the check-in time
   */
  void checkin(DateTime stamp=std::chrono::system_clock::now());

  /**
   * @brief Store the check-out time
   */
  void checkout(DateTime stamp=std::chrono::system_clock::now());

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
   * @param the source that holds the hotel information
   *
   * @return The recorded "Reservations" for the given room
   */
  static Reservations
  find_by_room(const std::string& room_id, void* src);


  /**
   * @brief Find the reservations associated to a guest
   *
   * @param guest identifier to retrieve the reservations for
   * @param the source that holds the hotel information
   *
   * @return The "Reservations" for the given guest
   */
  static Reservations
  find_by_guest(std::string_view guest_id, void* src);

  // state
  // =====
  std::string id;
  std::string guest_id;
  std::string room_id;
  Period period;
  std::optional<DateTime> checkin_at = std::nullopt;
  std::optional<DateTime> checkout_at = std::nullopt;
  std::string notes;

private:
  void* src;
};
