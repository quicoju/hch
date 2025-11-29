#include <exception>

#include "Room.hh"
#include "HotelData.hh"

const Amenities Room::amenities()
{
  if (!amenities_.size())
    amenities_ =  static_cast<RoomData*>(src)->amenities;

  return amenities_;
}

Room Room::find_by_id(string_view id, void* src)
{
  auto& rooms_data = static_cast<HotelData*>(src)->rooms;
  for (auto& r: rooms_data) {
    if (r.id == id)
      return {r.id, r.capacity, &r};
  }
  throw std::invalid_argument{ std::format("Room {} not found", id) };
}

Rooms Room::find_all(void* src)
{
  Rooms rooms{};
  auto& rooms_data = static_cast<HotelData*>(src)->rooms;

  for (auto& r: rooms_data) {
    rooms.emplace_back(r.id, r.capacity, &r); // TODO:maybe room data
  }
  return rooms;
}
