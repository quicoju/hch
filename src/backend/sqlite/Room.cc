#include <exception>

#include "Room.hh"
#include "SQLite.hh"

Room::Room(const std::string id, size_t capacity, void *data_source)
 : id{id}
 , capacity{capacity}
 , src{data_source}
 , amenities_{}
{}

Room::Room(const char *id, size_t capacity, void *data_source)
 : id{id}
 , capacity{capacity}
 , src{data_source}
 , amenities_{}
{}


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

void  Room::reserve(Date date, Duration dur)
{
  auto* db = static_cast<SQLite*>(src);
  auto date_ = _dstr(date);
  db->prepare(R"(
INSERT INTO reservations (room_id, begin_date, duration_days)
VALUES ((SELECT id FROM rooms WHERE name = ?), ?, ?)
)").execute(id, date_, dur.days());
}

void Room::cancel_reservation(Date date)
{
  auto* db = static_cast<SQLite*>(src);
  auto date_ = _dstr(date);
  db->prepare(R"(
DELETE FROM reservations
 WHERE room_id = (SELECT id FROM rooms WHERE name = ?)
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
  JOIN rooms r ON room_id = r.id
  WHERE name = ?
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

const Amenities Room::amenities()
{
  if (amenities_.size()) {
    std::cout << "Using the cache\n";
    return amenities_;
  }

  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT amenity_name
  FROM rooms
  JOIN rooms_amenities ON id = room_id
WHERE name = ?
)");
  stmt.bind(id);

  while(stmt.next()) {
    amenities_.emplace_back(stmt.get<std::string>());
  }

  return amenities_;
}
