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
