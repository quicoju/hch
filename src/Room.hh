#pragma once

#include "concepts.hh"

struct Room {
  Room(const std::string& id, size_t capacity, void* src)
    : id{id}
    , capacity{capacity}
    , src{src}
  {}

  Room(const std::string& id, size_t capacity, const Amenities& a, void* src)
    : id{id}
    , capacity{capacity}
    , src{src}
    , amenities_{a}
  {}

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

  /**
   * @brief Find a room by it's identifier
   *
   * @param Room identifier
   */
  static Room
  find_by_id(const std::string& id, void* src);

  /* Properties */
  /* ========== */
  const std::string id;
  const size_t capacity;

private:
  void* src;
  Amenities amenities_;
};
