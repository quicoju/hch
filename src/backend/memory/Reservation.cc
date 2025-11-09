#include <exception>

#include "Reservation.hh"
#include "HotelData.hh"
#include "concepts.hh"

std::string
Reservation::reserve(const std::string& guest_id,
                     const std::string& room_id,
                     Date date,
                     Duration dur,
                     std::string_view notes,
                     void* src)
{
  auto& reservations = static_cast<HotelData*>(src)->reservations;

  // TODO: create a type that can be injected
  // or user provided. Each business might have their
  // own rules to generate the reservation identifier
  auto next_id = reservations.size() + 1;
  std::string id = "W-000" + std::to_string(next_id);
  std::string n{notes};
  reservations.emplace_back(id, guest_id, room_id, Period{date, dur}, n);

  return id;
}

void Reservation::cancel()
{
  // TODO: This method shows the need of a new "status" field
  // in the reservation. Deleting the reservation without leaving
  // a track isn't a good idea
  auto& reservations = static_cast<HotelData*>(src)->reservations;
  reservations.remove_if([this](auto& r){ return r.id == id; });
}

Reservation
Reservation::find_by_id(const std::string& id, void* src)
{
  const auto& reservations = static_cast<HotelData*>(src)->reservations;
  for (const auto& r: reservations) {
    if (r.id == id)
      return {id, r.guest_id, r.room_id, r.period, r.notes, src};
  }
  throw std::invalid_argument{"Reservation " + id + " doesn't exist"};
}

Reservations
Reservation::find_by_room(const std::string& room_id, void* src)
{
  Reservations reservations{};
  const auto& all_reservations = static_cast<HotelData*>(src)->reservations;
  for (const auto& r: all_reservations) {
    if (r.room_id == room_id) {
      reservations.emplace_back(r.id, r.guest_id, r.room_id, r.period, r.notes, src);
    }
  }
  return reservations;
}
