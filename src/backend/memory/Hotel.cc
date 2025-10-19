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
  auto* data = static_cast<HotelData*>(src);
  const auto& room_agenda = Reservation::find_by_room(room.id, &(data->reservations));
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

  auto& agenda = static_cast<HotelData*>(src)->reservations;
  return Reservation::reserve(guest_id, r.id, d, dur, &agenda);
}

void Hotel::cancel(const std::string& id)
{
  auto& reservations = static_cast<HotelData*>(src)->reservations;
  Reservation::find_by_id(id, &reservations).cancel();
}


Reservations Hotel::reservations_for(const Room& r)
{
  auto& agenda = static_cast<HotelData*>(src)->reservations;
  return Reservation::find_by_room(r.id, &agenda);
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
