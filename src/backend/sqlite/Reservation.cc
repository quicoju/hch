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
SELECT reservation_id, g.email, ro.name, begin_date, duration_days, notes
  FROM reservations re
  LEFT JOIN guests  g ON g.id = guest_id
  LEFT JOIN rooms  ro ON ro.id = room_id
 WHERE reservation_id = ?
)");
    stmt.bind(id);

    if (!stmt.next())
      throw std::invalid_argument{"Reservation " + id + " doesn't exist"};

    return {
      stmt.get<std::string>(0),
      stmt.get<std::string>(1),
      stmt.get<std::string>(2),
      { from_string(stmt.get<std::string>(3)), Days{stmt.get<int>(4)} },
      stmt.get<std::string>(5),
      src,
    };
}

Reservations
Reservation::find_by_room(const std::string& room_id, void* src)
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT reservation_id, g.email, ro.name, begin_date, duration_days, notes
  FROM reservations re
  LEFT JOIN guests  g ON g.id = guest_id
  LEFT JOIN rooms  ro ON ro.id = room_id
 WHERE ro.name = ?
)");
  stmt.bind(room_id);

  Reservations reservations{};
  while (stmt.next()) {
    reservations.emplace_back(
      stmt.get<std::string>(0),
      stmt.get<std::string>(1),
      stmt.get<std::string>(2),
      Period{ from_string(stmt.get<std::string>(3)), Days{stmt.get<int>(4)} },
      stmt.get<std::string>(5),
      src
    );
  }

  return reservations;
}
