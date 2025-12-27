#include <exception>

#include "Reservation.hh"
#include "SQLite.hh"
#include "concepts.hh"

std::string
Reservation::reserve(string_view guest_id,
                     string_view room_id,
                     Date date,
                     Duration dur,
                     string_view notes,
                     Backend* db)
{
  auto stmt = db->prepare(R"(
SELECT IFNULL(MAX(id), 0)
  FROM reservations
)");

  stmt.next();
  auto next_id = stmt.get<int>() + 1;
  std::string id = "W-000" + std::to_string(next_id);

  auto stmt2 = db->prepare(R"(
INSERT INTO
reservations(id, reservation_id, guest_id, room_id, begin_date, duration_days, notes)
VALUES (?, ?,
  (SELECT id FROM guests WHERE email = ?),
  (SELECT id FROM rooms  WHERE name = ?),
  ?, ?, ?)
)");
  stmt2.execute(next_id, id, guest_id, room_id, date.as_string(), dur.count(), notes);
  return id;
}

void Reservation::cancel()
{
  auto stmt = src->prepare(R"(
DELETE FROM Reservations
 WHERE reservation_id = ?
)");
  stmt.execute(id);
}

template<typename T>
static const T& _update(string_view name, string_view id, T& value, Backend* db)
{
  auto stmt = db->prepare(std::format(R"(
UPDATE Reservations
   SET {} = ?
 WHERE reservation_id = ?)", name));
  stmt.execute(value, id);
  return value;
}
// this macro takes care of updating the value
// in the DB while also setting the object property
#define _UPDATE_(prop, val) prop = _update(#prop, id, val, src)

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

// TODO: instead of using this function, try to use a
// proper constructor
static Reservations
reservations_from(SQLite::Statement& stmt, Backend* src)
{
  Reservations reservations{};
  while (stmt.next()) {
    Reservation r {
      stmt.get<std::string>(0),
      stmt.get<std::string>(1),
      stmt.get<std::string>(2),
      { Date::from_string(stmt.get<std::string>(3)), Days{stmt.get<int>(4)} },
      stmt.get<std::string>(7),
      src,
    };
    r.checkin_at = stmt.get<std::optional<DateTime>>(5);
    r.checkout_at = stmt.get<std::optional<DateTime>>(6);
    reservations.push_back(std::move(r));
  }
  return reservations;
}

Reservations
find_by_condition(std::string_view cond, std::string_view value, Backend* db)
{
  auto query = std::format(R"(
SELECT reservation_id, email, rooms.name, begin_date, duration_days,
       checkin_at, checkout_at, notes
  FROM reservations re
  LEFT JOIN guests ON guests.id = guest_id
  LEFT JOIN rooms  ON rooms.id = room_id
 WHERE {} = ?)", cond);
  auto stmt = db->prepare(query);
  stmt.bind(value);
  return reservations_from(stmt, db);
}

Reservation
Reservation::find_by_id(std::string_view id, Backend* db)
{
  auto reservations = find_by_condition("reservation_id", id, db);
  if (reservations.empty())
    throw std::invalid_argument{std::format("Reservation {} doesn't exist", id)};
  return reservations.front();
}

Reservations
Reservation::find_by_room(string_view id, Backend* db)
{
  return find_by_condition("rooms.name", id, db);
}

Reservations
Reservation::find_by_guest(string_view id, Backend* db)
{
  return find_by_condition("guests.email", id, db);
}

Reservations
Reservation::find_by_starting_date(const Date& date, Backend* db)
{
  return find_by_condition("begin_date", date.as_string(), db);
}

Reservations
Reservation::find_by_ending_date(const Date& date, Backend* db)
{
  auto c = "date(begin_date, '+'||duration_days||' days')";
  return find_by_condition(c, date.as_string(), db);
}
