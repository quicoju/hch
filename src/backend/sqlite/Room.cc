#include <exception>

#include "Room.hh"
#include "SQLite.hh"

bool Room::is_available_on(Date date, Duration dur) const
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT COUNT(*)
  FROM reservations
  JOIN rooms r ON room_id = r.id
 WHERE name = ?
   AND date(?, '+' || ? || ' days') > begin_date
   AND ? < date(begin_date, '+' || duration_days || ' days')
)");

  auto date_ = _dstr(date);
  stmt.bind(id, date_, dur.days(), date_);

  return stmt.next()
    ? stmt.get<int>() == 0
    : false;
}

const std::list<Period> Room::reservations() const
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT begin_date, duration_days
  FROM reservations
  JOIN rooms r ON room_id = r.id
  WHERE name = ?
  ORDER BY begin_date
)");

  std::list<Period> l{};
  stmt.bind(id);

  while (stmt.next()) {
    auto date = from_string(stmt.get<std::string>());
    auto dur = stmt.get<int>(1);
    l.emplace_back(date, Days{dur});
  }

  return l;
}

const Amenities Room::amenities()
{
  if (amenities_.size())
    return amenities_;

  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT amenity_name
  FROM rooms
  JOIN rooms_amenities ON id = room_id
WHERE name = ?
ORDER by amenity_name
)");
  stmt.bind(id);

  while(stmt.next()) {
    amenities_.emplace(stmt.get<std::string>());
  }

  return amenities_;
}
