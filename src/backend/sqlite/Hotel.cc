#include "Hotel.hh"

#include <exception>

#include "SQLite.hh"

Hotel::Hotel(void *data_source) : src{data_source} {}

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const noexcept
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT COUNT(*) >= ?
  FROM rooms r
 WHERE r.id NOT IN (
    SELECT DISTINCT room_id
      FROM reservations
     WHERE room_id = r.id
       AND date(?, '+' || ? || ' days') > begin_date
       AND ? < date(begin_date, '+' || duration_days || ' days'))
)");

  auto date_ = _dstr(date);
  stmt.bind(n_rooms, date_, dur.days(), date_);

  return stmt.next()
    ? stmt.get<int>() != 0
    : false;
}

Rooms Hotel::find_available_on(Date d, Duration dur)
  const noexcept {

  Rooms available_rooms{};
  auto* db = static_cast<SQLite*>(src);

  try {
    auto stmt = db->prepare(R"(
SELECT r.id
  FROM rooms r
 WHERE r.id NOT IN (
     SELECT DISTINCT room_id
       FROM reservations
      WHERE room_id = r.id
       AND date(?, '+' || ? || ' days') > begin_date
       AND ? < date(begin_date, '+' || duration_days || ' days'))
)");

    auto date_ = _dstr(d);
    stmt.bind(date_, dur.days(), date_);

    while (stmt.next()) {
      auto room_id = stmt.get<std::string>();
      available_rooms.emplace_back(room_id, db);
    }
  }
  catch (const std::runtime_error& e) {
    // TODO: inspect the exception and do something useful with it
  }

  return available_rooms;
};

Room Hotel::room(const std::string id) {
  auto *db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT id FROM rooms
 WHERE id = ?
)");
  stmt.bind(id);

  if (stmt.next())
    return Room{stmt.get<std::string>(), db};

  throw std::invalid_argument{"Room" + id + " not found"};
}

Rooms Hotel::rooms() const
{
  auto *db = static_cast<SQLite*>(src);
  auto stmt = db->prepare("SELECT id FROM rooms");

  Rooms rooms{};
  while (stmt.next())
    rooms.emplace_back(stmt.get<std::string>(), db);

  return rooms;
}
