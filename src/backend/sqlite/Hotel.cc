#include "Hotel.hh"

#include <exception>

#include <sqlite3.h>

Hotel::Hotel(void *data_source)
  : src{data_source} {};

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const noexcept
{
  auto *db = static_cast<sqlite3*>(src);
  const char* sql = R"(
SELECT COUNT(*) >= ?
  FROM rooms r
 WHERE r.id NOT IN (
    SELECT DISTINCT room_id
      FROM reservations
     WHERE room_id = r.id
       AND date(?, '+' || ? || ' days') > begin_date
       AND ? < date(begin_date, '+' || duration_days || ' days'))
)";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;

  auto date_str = _dstr(date);
  auto dur_str = std::to_string(dur.days());

  const char* date_cstr = date_str.c_str();
  const char* dur_cstr = dur_str.c_str();

  sqlite3_bind_int64(stmt, 1, n_rooms);
  sqlite3_bind_text(stmt, 2, date_cstr, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, dur_cstr, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, date_cstr, -1, SQLITE_STATIC);

  bool result{false};

  if (sqlite3_step(stmt) == SQLITE_ROW)
    result = sqlite3_column_int(stmt, 0) != 0;

  sqlite3_finalize(stmt);
  return result;
}

Rooms Hotel::find_available_on(Date d, Duration dur)
  const noexcept {
  auto* db = static_cast<sqlite3*>(src);

  const char * sql = R"(
SELECT r.id
  FROM rooms r
 WHERE r.id NOT IN (
     SELECT DISTINCT room_id
       FROM reservations
      WHERE room_id = r.id
       AND date(?, '+' || ? || ' days') > begin_date
       AND ? < date(begin_date, '+' || duration_days || ' days'))
)";

  Rooms available_rooms;

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return available_rooms;

  auto date_str = _dstr(d);
  auto dur_str = std::to_string(dur.days());

  sqlite3_bind_text(stmt, 1, date_str.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, dur_str.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, date_str.c_str(), -1, SQLITE_STATIC);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::string room_id{(char*)sqlite3_column_text(stmt, 0)};
    available_rooms.emplace_back(room_id, db);
  }

  sqlite3_finalize(stmt);
  return available_rooms;
};

Room Hotel::room(const std::string id) {
  auto *db = static_cast<sqlite3*>(src);
  const char* sql = R"(
SELECT id FROM rooms
 WHERE id = ?
)";
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare statement");

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    auto id = (const char*)sqlite3_column_text(stmt, 0);
    Room r{id, db};
    // TODO: maybe we should finalize the statement also
    // when the romm wasn't found
    sqlite3_finalize(stmt);
    return r;
  }
  throw std::invalid_argument{"Room" + id + " not found"};
}

Rooms Hotel::rooms() const {
  auto *db = static_cast<sqlite3*>(src);
  const char* sql = R"(SELECT id FROM rooms)";

  Rooms rooms{};

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare statement");

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *id = (const char *) sqlite3_column_text(stmt, 0);
    rooms.emplace_back(id, db);
  }

  return rooms;
}
