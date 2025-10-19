#include <algorithm>

#include "Hotel.hh"
#include "Reservation.hh"

#include "HotelData.hh"

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const
{

  for (const auto& r: rooms()) {
    if (is_available_on(r, date, dur)) {
      --n_rooms;
      if (!n_rooms) return true;
    }
  }
  return false;
}

bool Hotel::is_available_on(Room room, Date date, Duration dur)
  const
{
  Period p{date, dur};
  const auto& room_agenda = Reservation::find_by_room(room.id, src);
  auto end = room_agenda.cend();

  return end == std::find_if(room_agenda.cbegin(), end,
      [&p](const auto& rsv) { return p.intersects(rsv.period); });
}

Rooms Hotel::find_available_on(Date d, Duration dur, Amenities amenities)
  const
{
  Rooms available_rooms{};

  for (auto& r: rooms()) {
    if (is_available_on(r, d, dur) && r.has_amenities(amenities))
      available_rooms.emplace_back(r);
  }
  return available_rooms;
}

std::string Hotel::reserve(const std::string& guest_id,
                           const Room& r,
                           Date d,
                           Duration dur)
{
  if (!is_available_on(r, d, dur))
    throw std::runtime_error{"Room is already reserved for overlapping dates"};

  return Reservation::reserve(guest_id, r.id, d, dur, src);
}

void Hotel::cancel(const std::string& id)
{
  Reservation::find_by_id(id, src).cancel();
}


Reservations Hotel::reservations_for(const Room& r)
{
  return Reservation::find_by_room(r.id, src);
}


Room Hotel::room(const std::string& id)
{
  return Room::find_by_id(id, src);
}

Rooms Hotel::rooms() const
{
  auto* hotel_data =  static_cast<HotelData*>(src);
  Rooms rooms{};

  for (auto& d: hotel_data->rooms) {
    Room r{d.id, d.capacity, &d};
    rooms.emplace_back(r);
  }

  return rooms;
}

const RateReport
Hotel::rate_report_for(Room room, Date _, Duration dur) const
{
  return Rate::Calculator{src}.rate_report_for(room, _, dur);
}
