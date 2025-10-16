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
