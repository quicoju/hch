#include <exception>

#include "Reservation.hh"
#include "SQLite.hh"
#include "concepts.hh"

std::string
Reservation::reserve(const std::string& guest_id,
                     const std::string& room_id,
                     Date date,
                     Duration dur,
                     std::string_view notes,
                     void* src)
{
  auto* db = static_cast<SQLite*>(src);
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
  stmt2.execute(next_id, id, guest_id, room_id, _dstr(date), dur.days(), notes);
  return id;
}

void Reservation::cancel()
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
DELETE FROM Reservations
 WHERE reservation_id = ?
)");
  stmt.execute(id);
}

Reservation
Reservation::find_by_id(const std::string& id, void* src)
{
    auto* db = static_cast<SQLite*>(src);
    auto stmt = db->prepare(R"(
SELECT reservation_id, g.email, ro.name, begin_date, duration_days,
       checkin_at, checkout_at, notes
  FROM reservations re
  LEFT JOIN guests  g ON g.id = guest_id
  LEFT JOIN rooms  ro ON ro.id = room_id
 WHERE reservation_id = ?
)");
    stmt.bind(id);

    if (!stmt.next())
      throw std::invalid_argument{"Reservation " + id + " doesn't exist"};

    Reservation r{
      stmt.get<std::string>(0),
      stmt.get<std::string>(1),
      stmt.get<std::string>(2),
      { from_string(stmt.get<std::string>(3)), Days{stmt.get<int>(4)} },
      stmt.get<std::string>(7),
      src,
    };
    r.checkin_at = stmt.get<std::optional<DateTime>>(5);
    r.checkout_at = stmt.get<std::optional<DateTime>>(6);
    return r;
}

Reservations
find_by_column(std::string_view cond, std::string_view id, void* src)
{
  auto* db = static_cast<SQLite*>(src);
  std::stringstream query;
  query << R"(
SELECT reservation_id, email, rooms.name, begin_date, duration_days,
       checkin_at, checkout_at, notes
  FROM reservations re
  LEFT JOIN guests ON guests.id = guest_id
  LEFT JOIN rooms  ON rooms.id = room_id
 WHERE )" << cond << " = ?";
  auto stmt = db->prepare(query.str());
  stmt.bind(id);

  Reservations reservations{};
  while (stmt.next()) {
    Reservation r {
      stmt.get<std::string>(0),
      stmt.get<std::string>(1),
      stmt.get<std::string>(2),
      Period{ from_string(stmt.get<std::string>(3)), Days{stmt.get<int>(4)} },
      stmt.get<std::string>(7),
      src
    };
    r.checkin_at = stmt.get<std::optional<DateTime>>(5);
    r.checkout_at = stmt.get<std::optional<DateTime>>(6);
    reservations.push_back(std::move(r));
  }
  return reservations;
}

Reservations
Reservation::find_by_room(const std::string& id, void* src)
{
  return find_by_column("rooms.name", id, src);
}

Reservations
Reservation::find_by_guest(std::string_view id, void* src)
{
  return find_by_column("guests.email", id, src);
}
