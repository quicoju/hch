#include <exception>

#include "Reservation.hh"
#include "HotelData.hh"
#include "concepts.hh"

Reservation::Reservation(const std::string& id, void* src)
    : src{ src }
    , id{ id }
    , period{ Period{Today, Days{1}} } // TODO: temporary default
  {
    auto* reservations = static_cast<ReservationsData*>(src);
    for (const auto& r: *reservations) {
      if (r.id == id) {
        guest_id = r.guest_id;
        room_id = r.room_id;
        period = r.period;
        return;
      }
    }
    throw std::runtime_error{"Reservation " + id + " not found"};
  }


// static methods
// ==============
Reservations_
Reservation::find_by_room(const std::string& room_id, void* src)
{
  Reservations_ reservations{};
  auto* all_reservations = static_cast<ReservationsData*>(src);
  for (const auto& r: *all_reservations) {
    if (r.room_id == room_id) {
      // TODO: we need a constructor that takes all the reservation
      // properties. With the current setup we're searching the collection
      // twice, the first one here to match the room_id and the second
      // time on the constructor to validate that the given reservation
      // exists.
      reservations.emplace_back(r.id, src);
    }
  }
  return reservations;
}
