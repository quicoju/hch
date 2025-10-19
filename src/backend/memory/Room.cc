#include <exception>

#include "Room.hh"
#include "HotelData.hh"

const Reservations Room::reservations() const
{
  auto* room_data = static_cast<RoomData*>(src);
  Reservations reservations{};

  for (const auto r: room_data->reservations)
    reservations.emplace_back(r);

  return reservations;
}

const Amenities Room::amenities()
{
  if (!amenities_.size())
    amenities_ =  static_cast<RoomData*>(src)->amenities;

  return amenities_;
}
