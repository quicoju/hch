#include <exception>

#include "Reservation.hh"
#include "SQLite.hh"
#include "concepts.hh"

Reservation
Reservation::find_by_id(const std::string& id, void* src)
{
    auto* db = static_cast<SQLite*>(src);
    auto stmt = db->prepare(R"(
SELECT reservation_id, room_id, guest_id, begin_date, duration_days
  FROM reservations
 WHERE reservation_id = ?
)");
    std::cout << "Searching for " << id << std::endl;
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
