#include "Room.hh"

#include <exception>

#include <sqlite3.h>
Room::Room(const char* id, void *data_source)
 : id{id}
 , src{data_source} {}

Room::Room(const std::string id, void *data_source)
  : id{id}
  , src{data_source} {}

bool Room::is_available_on(Date date, Duration dur) const
{
  auto *db = static_cast<sqlite3*>(src);
  const char* sql = R"(
SELECT COUNT(*)
  FROM reservations
 WHERE room_id = ?
   AND (-- New period starts during existing reservation
         (? >= begin_date  AND ? < date(begin_date, '+' || duration_days || ' days'))
       OR -- New period ends during existing reservation
         (date(?, '+' || ? || ' days') > begin_date  AND date(?, '+' || ? || ' days') <= date(begin_date, '+' || duration_days || ' days'))
       OR -- New period spans over existing reservation
         (? <= begin_date AND date(?, '+' || ? || ' days') >= date(begin_date, '+' || duration_days || ' days'))
))";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error{sqlite3_errmsg(db)};
    }

    // Bind parameters
    std::string date_str = str2(date);
    std::string dur_str = std::to_string(dur.days());

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC); // room_id
    sqlite3_bind_text(stmt, 2, date_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, date_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, date_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, dur_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, date_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, dur_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, date_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, date_str.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, dur_str.c_str(), -1, SQLITE_STATIC);

    bool available = true;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        available = sqlite3_column_int(stmt, 0) == 0; // 0 conflicts means available
    }

    sqlite3_finalize(stmt);
    return available;
}

void  Room::reserve(Date date, Duration dur)
{
  auto* db = static_cast<sqlite3*>(src);
  const char* sql = R"(
INSERT INTO reservations (room_id, begin_date, duration_days)
VALUES (?, ?, ?)
)";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare statement");
  }

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, str2(date).c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, dur.days());

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw std::logic_error{ std::string{sqlite3_errmsg(db)} };
  }
  sqlite3_finalize(stmt);
}

void Room::cancel_reservation(Date date)
{
  auto* db = static_cast<sqlite3*>(src);

  // Find reservations that contain this date
  const char* sql = R"(
DELETE FROM reservations
 WHERE room_id = ?
   AND begin_date <= ?
   AND date(begin_date, '+' || duration_days || ' days') > ?
)";

  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare statement");

  std::string date_str = str2(date);
  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, date_str.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, date_str.c_str(), -1, SQLITE_TRANSIENT);

  int result = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (result != SQLITE_DONE)
    throw std::runtime_error("Failed to cancel reservation");
}

const std::list<Reservation> Room::reservations() const
{
  auto* db = static_cast<sqlite3*>(src);

  const char* sql = R"(
SELECT begin_date, duration_days
  FROM reservations
  WHERE room_id = ?
  ORDER BY begin_date
)";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare statement");

  std::list<Reservation> l{};

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const  char *begin_str = (const char*)sqlite3_column_text(stmt, 0);
    int duration_days = sqlite3_column_int(stmt, 1);

    Date begin = from_string(begin_str);
    l.emplace_back(begin, Days{duration_days});
  }

  sqlite3_finalize(stmt);
  return l;
}
