#include <exception>

#include "Room.hh"
#include "SQLite.hh"

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


Room Room::find_by_id(const std::string& id, void* src)
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT name, capacity
  FROM rooms
 WHERE name = ?
)");
  stmt.bind(id);

  if (stmt.next()) {
    auto name = stmt.get<std::string>();
    auto capacity = stmt.get<size_t>(1);
    return Room{name, capacity, db};
  }
  throw std::invalid_argument{std::string{"Room "} + id + " not found"};
}

Rooms Room::find_all(void* src)
{
  Rooms rooms{};
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT name, capacity
  FROM rooms
)");

  while (stmt.next()) {
    auto room_id = stmt.get<std::string>();
    auto capacity = stmt.get<size_t>(1);
    rooms.emplace_back(room_id, capacity, db);
  }
  return rooms;
}
