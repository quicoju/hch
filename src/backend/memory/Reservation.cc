#include <exception>

#include "Reservation.hh"
#include "HotelData.hh"
#include "concepts.hh"

std::string
Reservation::reserve(string_view guest_id,
                     string_view room_id,
                     Date date,
                     size_t days,
                     string_view notes,
                     Backend* src)
{
  auto& reservations = src->reservations;

  // TODO: create a type that can be injected
  // or user provided. Each business might have their
  // own rules to generate the reservation identifier
  auto next_id = reservations.size() + 1;
  std::string id = "W-000" + std::to_string(next_id);
  std::string n{notes};
  reservations.emplace_back(id, guest_id, room_id, date, days, n);

  return id;
}

void Reservation::cancel()
{
  // TODO: This method shows the need of a new "status" field
  // in the reservation. Deleting the reservation without leaving
  // a track isn't a good idea
  src->reservations.remove_if([this](auto& r){ return r.id == id; });
}

// TODO: this method reveals the need of a better way to interact
// with the backend. One idea is that this implementation inherits
// from a base "BACKEND" class that implements the details of
// interacting with the actual data
inline auto
_from_store(std::string_view id, Backend* src)
{
  return std::ranges::find_if(src->reservations,
    [id](const auto& r){ return r.id == id; });
}
// this macro takes care of updating the value
// in the store while also setting the object property
#define _UPDATE_(prop, val) _from_store(id, src)->prop = prop = val

void Reservation::checkin(DateTime stamp)
{
  _UPDATE_(checkin_at, stamp);
}

void Reservation::checkout(DateTime stamp)
{
  _UPDATE_(checkout_at, stamp);
}

void Reservation::annotate(std::string_view n)
{
  _UPDATE_(notes, n);
}

Reservation
Reservation::find_by_id(std::string_view id, Backend* src)
{
  for (const auto& r: src->reservations) {
    if (r.id == id) {
      Reservation rsv {id, r.guest_id, r.room_id, {r.start, r.days}, r.notes, src};
      rsv.checkin_at = r.checkin_at;
      rsv.checkout_at = r.checkout_at;
      return rsv;
    }
  }
  throw std::invalid_argument{std::format("Reservation {} doesn't exist", id)};
}

static Reservations
find_by_predicate(std::function<bool(const ReservationData&)> p, Backend* src)
{
  Reservations reservations{};
  for (const auto& r: src->reservations) {
    if (p(r)) {
      Reservation rsv{r.id, r.guest_id, r.room_id, {r.start, r.days}, r.notes, src};
      rsv.checkin_at = r.checkin_at;
      rsv.checkout_at = r.checkout_at;
      reservations.push_back(std::move(rsv));
    }
  }
  return reservations;
}

Reservations
Reservation::find_by_room(string_view id, Backend* src)
{
  auto p = [id](const auto& r){ return r.room_id == id; };
  return find_by_predicate(p, src);
}

Reservations
Reservation::find_by_guest(string_view id, Backend* src)
{
  auto p = [id](const auto& r){ return r.guest_id == id; };
  return find_by_predicate(p, src);
}

Reservations
Reservation::find_by_starting_date(const Date& date, Backend* src)
{
  auto p = [date](const auto& r){ return r.start == date; };
  return find_by_predicate(p, src);
}

Reservations
Reservation::find_by_ending_date(const Date& date, Backend* src)
{
  auto p = [date](const auto& r){ return Period{r.start, r.days}.end() == date; };
  return find_by_predicate(p, src);
}
