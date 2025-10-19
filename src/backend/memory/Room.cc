#include <exception>

#include "Room.hh"
#include "HotelData.hh"

const Amenities Room::amenities()
{
  if (!amenities_.size())
    amenities_ =  static_cast<RoomData*>(src)->amenities;

  return amenities_;
}
