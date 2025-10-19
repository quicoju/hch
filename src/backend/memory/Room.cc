#include <exception>

#include "Room.hh"
#include "HotelData.hh"

bool Room::is_available_on(Date date, Duration dur) const
{
  const Period p{date, dur};
  const auto& agenda = static_cast<RoomData*>(src)->reservations;
  auto end = agenda.cend();

  return end == std::find_if(agenda.cbegin(), end,
      [&p](const Period& r){ return r.intersects(p); } );
}

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
