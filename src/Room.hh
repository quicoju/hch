#pragma once

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

  /**
   * @brief cancel the whole reservation associated to the given Date
   *
   * If a reservation matchies the given Date, the whole reservation
   * is cancelled, note that a reservation might span more multiple days.
   *
   * If the given day doesn't match a reservation then this methods
   * /silently/ does nothing.
   *
   * @param Date that will match the associated reservation
   */
  void cancel_reservation(Date);

  /**
   * @brief Provide a view of the agenda.
   *
   * The view is read only, it cannot be modified.
   */
  const std::list<Reservation>& reservations() const;

  /* Properties */
  /* ========== */
  const std::string id;

private:
  std::list<Reservation> agenda;
};

