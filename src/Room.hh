#pragma once

#include "concepts.hh"

struct Room {
  Room(const std::string& id, size_t capacity, void* src)
    : id{id}
    , capacity{capacity}
    , src{src}
  {}

  /**
   * @brief Provide a view of the agenda.
   *
   * The view is read only, it cannot be modified.
   */
  const Reservations reservations() const;

 /**
   * @brief Provide a view of the room amenities
   *
   * The view is read only, it cannot be modified.
   */
  const Amenities amenities();

  /**
   * @brief Returns true if the room provides  the given amenities
   */
  const bool has_amenities(const Amenities& amenities);

  /* Properties */
  /* ========== */
  const std::string id;
  const size_t capacity;

private:
  void* src;
  Amenities amenities_;
};
