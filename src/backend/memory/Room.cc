#include <exception>

#include "Room.hh"
#include "HotelData.hh"

const Amenities Room::amenities()
{
  if (!amenities_.size())
    amenities_ =  static_cast<RoomData*>(src)->amenities;

  return amenities_;
}

Room Room::find_by_id(const std::string& id, void* src)
{
  const auto& rooms = static_cast<HotelData*>(src)->rooms;
  for (const auto& r: rooms) {
    if (r.id == id)
      return {r.id, r.capacity, r.amenities, src}; // TODO: maybe room data
  }
  throw std::invalid_argument{std::string{"Room "} + id + " not found"};
}

Rooms Room::find_all(void* src)
{
  Rooms rooms{};
  const auto& rooms_data = static_cast<HotelData*>(src)->rooms;

  for (const auto& r: rooms_data) {
    rooms.emplace_back(r.id, r.capacity, r.amenities, src); // TODO:maybe room data
  }
  return rooms;
}
