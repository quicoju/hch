#include <exception>

#include "Hotel.hh"
#include "Reservation.hh"

#include "SQLite.hh"

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const
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

bool Hotel::is_available_on(Room room, Date date, Duration dur)
  const
{
  Period p{date, dur};
  auto room_agenda = Reservation::find_by_room(room.id, src);
  auto end = room_agenda.cend();

  return end == std::find_if(room_agenda.cbegin(), end,
      [&p](const auto& rsv) { return p.intersects(rsv.period); });
}


Rooms Hotel::find_available_on(Date d, Duration dur, Amenities amenities)
  const
{

  Rooms available_rooms{};
  auto* db = static_cast<SQLite*>(src);

  try {
    auto stmt = db->prepare(R"(
SELECT r.name, capacity
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
      auto capacity = stmt.get<size_t>(1);

      // TODO: it might be better to create a dynamic query that takes a
      // number of amenities and filters the rooms directly in the database
      Room r{room_id, capacity, db};
      if (r.has_amenities(amenities))
        available_rooms.push_back(std::move(r));
    }
  }
  catch (const std::runtime_error& e) {
    // TODO: inspect the exception and do something useful with it
  }

  return available_rooms;
};

std::string Hotel::reserve(const std::string& guest_id,
                           const Room& r,
                           Date d,
                           Duration dur)
{
  if (!is_available_on(r, d, dur))
    throw std::runtime_error{"Room is already reserved for overlapping dates"};

  return Reservation::reserve(guest_id, r.id, d, dur, src);
}

void Hotel::cancel(const std::string& id)
{
  Reservation::find_by_id(id, src).cancel();
}

Reservations Hotel::reservations_for(const Room& r)
{
  return Reservation::find_by_room(r.id, src);
}

Room Hotel::room(const std::string name) {
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT name, capacity FROM rooms
 WHERE name = ?
)");
  stmt.bind(name);

  if (stmt.next()) {
    auto name = stmt.get<std::string>();
    auto capacity = stmt.get<size_t>(1);
    return Room{name, capacity, db};
  }

  throw std::invalid_argument{"Room " + name + " not found"};
}

Rooms Hotel::rooms() const
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT name, capacity
  FROM rooms
)");

  Rooms rooms{};
  while (stmt.next()) {
    auto room_id = stmt.get<std::string>();
    auto capacity = stmt.get<size_t>(1);
    rooms.emplace_back(room_id, capacity, db);
  }

  return rooms;
}

const RateReport
Hotel::rate_report_for(Room room, Date _, Duration dur) const {
  auto calc = Rate::Calculator{src};
  return calc.rate_report_for(room, _, dur);
}
