#pragma once

#include "concepts.hh"
#include "Guest.hh"
#include "Room.hh"
#include "RateCalculator.hh"
#include "Reservation.hh"

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

  ////////////
  // Guests //
  ////////////

  /**
   * @brief records a new guest or does nothing if it already exists
   *
   * @param The E-mail address of the guest
   *
   * @return The identifier for the new/existing guest
   */
  std::string record_guest(std::string_view email);

  //////////////////
  // Reservations //
  //////////////////

  /**
   * @brief make a new reservation
   *
   * @param The Guest that is requresting the reservation
   * @param The Room to be reserved
   * @param Date when the room is to be reserved (defaults to Today)
   * @param Duration in days that the room is to be reserved (defaults to one)
   */
  std::string reserve(string_view guest,
                      string_view room,
                      Date date=Today,
                      Duration dur=Days{1});

  /**
   * @brief Cancel a reservation by identifier
   *
   * @param Reservation identifier
   */
  void cancel(const std::string& id);

  /**
   * @brief Check-in a reservation.
   *
   * @param Reservation identifier
   * @param UTC stamp to record the check-in. If none is given, set it to NOW
   */
  void checkin(std::string_view id, std::optional<DateTime> utc_stamp=std::nullopt);

  /**
   * @brief Check-out a reservation
   *
   * @param Reservation identifier
   * @param UTC stamp to record the check-out. If none is given, set it to NOW
   */
  void checkout(std::string_view id, std::optional<DateTime> utc_stamp=std::nullopt);

  /**
   * @brief Return a reservation matched by its identifier
   *
   * @param The reservation identifier
   *
   * @return The matched Reservation
   */
  Reservation reservation(string_view id) const;

  /**
   * @brief Find the reservations for a room
   *
   * @param The room identifier to look the reservations for
   *
   * @return The list of reservations
   */
  Reservations room_reservations(string_view room);

  /**
   * @brief Find the reservations for a guest
   *
   * @param The Guest to look the reservations for
   *
   * @return The list of reservations
   */
  Reservations guest_reservations(string_view guest);

  /**
   * @brief Find the reservations starting on a given date
   *
   * @param date of the "expected" check-in date, defaults to TODAY
   *
   * @return the list of the matching reservations
   */
  Reservations reservations_starting_on(const Date& d=Today);

  /**
   * @brief Find the reservations due on a given date
   *
   * @param date of the "expected" check-out date, defaults to TODAY
   *
   * @return the list of the matching reservations
   */
  Reservations reservations_ending_on(const Date& d=Today);

  /**
   * @brief show all the notes associated to a reservation
   *
   * @param The Reservation identifier
   *
   * @return a string with the Reservation notes
   */
  std::string reservation_notes(std::string_view id) const;

  /**
   * @brief modify the reservation notes by "title""
   *
   * The notes are organized as title/content (key/value) pairs,
   * each pair is a note.
   * One note can be edited by providing its title (key) and the
   * new content (value).
   *
   * POLICY: The note titled `Hotel::RATES_NOTE` note cannot be modified.
   *
   * @param The identifier of the reservation
   * @param The title of the note to patch
   * @param The new content of the note
   */
  void patch_reservation_notes(string_view id, string_view title, string_view content);

  ///////////
  // Rooms //
  ///////////

  /**
   * @brief Returns the room identified with ID
   *
   * @param The room identifier
   *
   */
  Room room(string_view id);

  /**
   * @brief Return all the rooms
   *
   */
  Rooms rooms() const;


  /**
   * @brief Provide a view of the room amenities
   *
   * @param The room identifier
   *
   * @return The room amenities
   */
  const Amenities room_amenities(string_view id);

  /**
   * @brief provide rate report associated to the room
   *
   * @param Room identifier
   * @param Date for the rate to be calculated (defaults to Today)
   * @param Number of days for the rate to be calculated (defaults to 1)
   */
  const RateReport
  rate_report_for(string_view room, Date date=Today, Duration dur=Days{1});


  // Note that can't be patched (see patch_reservation_notes)
  static string_view RATES_NOTE;

private:
  void* src;
};
