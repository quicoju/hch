#include "Room.hh"

#include <exception>

#include "SQLite.hh"

Room::Room(const char* id, void *data_source)
 : id{id}
 , src{data_source} {}

Room::Room(const std::string id, void *data_source)
  : id{id}
  , src{data_source} {}

bool Room::is_available_on(Date date, Duration dur) const
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT COUNT(*)
  FROM reservations
 WHERE room_id = ?
   AND date(?, '+' || ? || ' days') > begin_date
   AND ? < date(begin_date, '+' || duration_days || ' days')
)");

  auto date_ = _dstr(date);
  stmt.bind(id, date_, dur.days(), date_);

  return stmt.next()
    ? stmt.get<int>() == 0
    : false;
}

void  Room::reserve(Date date, Duration dur)
{
  auto* db = static_cast<SQLite*>(src);
  auto date_ = _dstr(date);
  db->prepare(R"(
INSERT INTO reservations (room_id, begin_date, duration_days)
VALUES (?, ?, ?)
)").execute(id, date_, dur.days());
}

void Room::cancel_reservation(Date date)
{
  auto* db = static_cast<SQLite*>(src);
  auto date_ = _dstr(date);
  db->prepare(R"(
DELETE FROM reservations
 WHERE room_id = ?
   AND begin_date <= ?
   AND date(begin_date, '+' || duration_days || ' days') > ?
)").execute(id, date_, date_);
}

const std::list<Reservation> Room::reservations() const
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT begin_date, duration_days
  FROM reservations
  WHERE room_id = ?
  ORDER BY begin_date
)");

  std::list<Reservation> l{};
  stmt.bind(id);

  while (stmt.next()) {
    auto date = from_string(stmt.get<std::string>());
    auto dur = stmt.get<int>(1);
    l.emplace_back(date, Days{dur});
  }

  return l;
}
