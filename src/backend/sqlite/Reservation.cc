#include <exception>

#include "Reservation.hh"
#include "SQLite.hh"
#include "concepts.hh"

Reservation
Reservation::find_by_id(const std::string& id, void* src)
{
    auto* db = static_cast<SQLite*>(src);
    auto stmt = db->prepare(R"(
SELECT reservation_id, g.email, ro.name, begin_date, duration_days
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
      src,
    };
}

Reservations_
Reservation::find_by_room(const std::string& room_id, void* src)
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT reservation_id, g.email, ro.name, begin_date, duration_days
  FROM reservations re
  LEFT JOIN guests  g ON g.id = guest_id
  LEFT JOIN rooms  ro ON ro.id = room_id
 WHERE ro.name = ?
)");
  stmt.bind(room_id);

  Reservations_ reservations{};
  while (stmt.next()) {
    reservations.emplace_back(
      stmt.get<std::string>(0),
      stmt.get<std::string>(1),
      stmt.get<std::string>(2),
      Period{ from_string(stmt.get<std::string>(3)), Days{stmt.get<int>(4)} },
      src
    );
  }

  return reservations;
}
