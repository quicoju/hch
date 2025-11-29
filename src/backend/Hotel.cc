#include <algorithm>

#include "Hotel.hh"
#include "Annotate.hh"
#include "Reservation.hh"

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

///////////
// Guest //
///////////
std::string Hotel::record_guest(std::string_view email)
{
  return Guest::record(email, src);
}

/////////////////
// Reservation //
/////////////////
string_view Hotel::RATES_NOTE = "Rates";

std::string Hotel::reserve(const std::string& guest_id,
                           const Room& r,
                           Date d,
                           Duration dur)
{
  if (!is_available_on(r, d, dur))
    throw std::runtime_error{"Room is already reserved for overlapping dates"};

  // POLICY: save the current rates in the reservation. This is important if
  // the rates change between the reservation time and the check-out time.
  // We need to honor the original prices
  auto report = rate_report_for(r.id, d, dur);
  auto note = annotate::as_string(std::map{std::pair{RATES_NOTE, report}});
  return Reservation::reserve(guest_id, r.id, d, dur, note, src);
}

void Hotel::cancel(const std::string& id)
{
  Reservation::find_by_id(id, src).cancel();
}

void Hotel::checkin(std::string_view id, std::optional<DateTime> utc_stamp)
{
  auto stamp = utc_stamp.value_or(std::chrono::system_clock::now());
  Reservation::find_by_id(id, src).checkin(stamp);
}

void Hotel::checkout(std::string_view id, std::optional<DateTime> utc_stamp)
{
  auto stamp = utc_stamp.value_or(std::chrono::system_clock::now());
  Reservation::find_by_id(id, src).checkout(stamp);
}

Reservation Hotel::reservation(std::string_view id) const
{
  return Reservation::find_by_id(id, src);
}

Reservations Hotel::room_reservations(string_view room_id)
{
  return Reservation::find_by_room(room_id, src);
}

Reservations Hotel::guest_reservations(std::string_view guest_id)
{
  return Reservation::find_by_guest(guest_id, src);
}

Reservations Hotel::reservations_starting_on(const Date& date)
{
  return Reservation::find_by_starting_date(date, src);
}

Reservations Hotel::reservations_ending_on(const Date& date)
{
  return Reservation::find_by_ending_date(date, src);
}

std::string Hotel::reservation_notes(std::string_view id) const
{
  return Reservation::find_by_id(id, src).notes;
}

void
Hotel::patch_reservation_notes(string_view id, string_view title, string_view content)
{
  if (title == RATES_NOTE) throw std::invalid_argument{
      std::format("The '{}' note can't be changed", RATES_NOTE)};
  auto reservation = Reservation::find_by_id(id, src);
  auto notes = annotate::read(reservation.notes);
  notes[title] = content;
  reservation.annotate(annotate::as_string(notes));
}

//////////
// Room //
//////////

Room Hotel::room(string_view id)
{
  return Room::find_by_id(id, src);
}

Rooms Hotel::rooms() const
{
  return Room::find_all(src);
}

const RateReport
Hotel::rate_report_for(string_view id, Date _, Duration dur)
{
  auto r = room(id);
  return Rate::Calculator{src}.rate_report_for(r, _, dur);
}
