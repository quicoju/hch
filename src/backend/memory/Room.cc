#include <exception>

#include "Room.hh"
#include "HotelData.hh"

const Amenities Room::amenities()
{
  if (!amenities_.size())
    amenities_ =  static_cast<RoomData*>(src)->amenities;

  return amenities_;
}

Room Room::find_by_id(string_view id, Backend* src)
{
  for (auto& r: src->rooms) {
    if (r.id == id)
      return {r.id, r.capacity, &r};
  }
  throw std::invalid_argument{ std::format("Room {} not found", id) };
}

Rooms Room::find_all(Backend* src)
{
  Rooms rooms{};
  for (auto& r: src->rooms)
    rooms.emplace_back(r.id, r.capacity, &r); // TODO:maybe hotel data

  return rooms;
}
