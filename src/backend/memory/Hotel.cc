#include <algorithm>

#include "Hotel.hh"
#include "Reservation.hh"

#include "HotelData.hh"

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const
{

  for (const auto& r: rooms()) {
    if (r.is_available_on(date, dur)) {
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
  auto* data = static_cast<HotelData*>(src);
  auto room_agenda = Reservation::find_by_room(room.id, &(data->reservations));
  auto end = room_agenda.cend();

  return end == std::find_if(room_agenda.cbegin(), end,
      [&p](const auto& rsv) { return p.intersects(rsv.period); });
}

Rooms Hotel::find_available_on(Date d, Duration dur, Amenities amenities)
  const
{
  Rooms available_rooms{};

  for (auto& r: rooms()) {
    if (r.is_available_on(d, dur) && r.has_amenities(amenities))
      available_rooms.emplace_back(r);
  }
  return available_rooms;
}

Room Hotel::room(const std::string id)
{
  for (auto& r : rooms())
    if (r.id == id) return r;
  throw std::invalid_argument{std::string{"Room "} + id + " not found"};
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
  auto* hotel_data = static_cast<HotelData*>(src);
  auto calc = Rate::Calculator{&hotel_data->rates};
  return calc.rate_report_for(room, _, dur);
}
