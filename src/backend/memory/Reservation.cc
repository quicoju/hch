#include <exception>

#include "Reservation.hh"
#include "HotelData.hh"
#include "concepts.hh"

Reservation
Reservation::find_by_id(const std::string& id, void* src)
{
    auto* reservations = static_cast<ReservationsData*>(src);
    for (const auto& r: *reservations) {
      if (r.id == id)
        return {id, r.guest_id, r.room_id, r.period, src};
    }
    throw std::invalid_argument{"Reservation " + id + " doesn't exist"};
}


Reservations_
Reservation::find_by_room(const std::string& room_id, void* src)
{
  Reservations_ reservations{};
  auto* all_reservations = static_cast<ReservationsData*>(src);
  for (const auto& r: *all_reservations) {
    if (r.room_id == room_id) {
      reservations.emplace_back(r.id, r.guest_id, r.room_id, r.period, src);
    }
  }
  return reservations;
}
