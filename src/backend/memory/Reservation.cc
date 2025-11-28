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

// TODO: this method reveals the need of a better way to interact
// with the backend. One idea is that this implementation inherits
// from a base "BACKEND" class that implements the details of
// interacting with the actual data
inline auto
_from_store(std::string_view id, void* src)
{
  auto& reservations = static_cast<HotelData*>(src)->reservations;
  return std::ranges::find_if(reservations,
    [id](const auto& r){ return r.id == id; });
}

void Reservation::checkin(DateTime stamp)
{
  // store the property in the object and in the data storage
  _from_store(id, src)->checkin_at = checkin_at = stamp;
}

void Reservation::checkout(DateTime stamp)
{
  _from_store(id, src)->checkout_at = *checkout_at = stamp;
}

void Reservation::annotate(std::string_view note)
{
  _from_store(id, src)->notes = notes = note;
}

Reservation
Reservation::find_by_id(std::string_view id, void* src)
{
  const auto& reservations = static_cast<HotelData*>(src)->reservations;
  for (const auto& r: reservations) {
    if (r.id == id) {
      Reservation rsv {id, r.guest_id, r.room_id, r.period, r.notes, src};
      rsv.checkin_at = r.checkin_at;
      rsv.checkout_at = r.checkout_at;
      return rsv;
    }
  }
  throw std::invalid_argument{std::format("Reservation {} doesn't exist", id)};
}

static Reservations
find_by_predicate(std::function<bool(const ReservationData&)> p, void* src)
{
  Reservations reservations{};
  const auto& all_reservations = static_cast<HotelData*>(src)->reservations;
  for (const auto& r: all_reservations) {
    if (p(r)) {
      Reservation rsv{r.id, r.guest_id, r.room_id, r.period, r.notes, src};
      rsv.checkin_at = r.checkin_at;
      rsv.checkout_at = r.checkout_at;
      reservations.push_back(std::move(rsv));
    }
  }
  return reservations;
}

Reservations
Reservation::find_by_room(const std::string& id, void* src)
{
  auto p = [id](const auto& r){ return r.room_id == id; };
  return find_by_predicate(p, src);
}

Reservations
Reservation::find_by_guest(std::string_view id, void* src)
{
  auto p = [id](const auto& r){ return r.guest_id == id; };
  return find_by_predicate(p, src);
}

Reservations
Reservation::find_by_starting_date(const Date& date, void* src)
{
  auto p = [date](const auto& r){ return r.period.begin() == date; };
  return find_by_predicate(p, src);
}

Reservations
Reservation::find_by_ending_date(const Date& date, void* src)
{
  auto p = [date](const auto& r){ return r.period.end() == date; };
  return find_by_predicate(p, src);
}
