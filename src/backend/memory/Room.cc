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

void Room::cancel_reservation(Date date)
{
  const Period p{date, Days{1}};
  auto& agenda = static_cast<RoomData *>(src)->reservations;
  auto end = agenda.end();

  auto match = std::find_if(agenda.begin(), end,
      [&p](Period& r){ return r.intersects(p); } );

  if (match != end)
    agenda.erase(match);
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
