#include "Room.hh"

/**
 * @brief This module contains common Room code for all backends
 */

const bool Room::has_amenities(const Amenities& list)
{
  auto& room_amenities = amenities();
  for (auto& amenity: list)
    if (!room_amenities.contains(amenity))
      return false;

  return true;
}
