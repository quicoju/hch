#include <exception>

#include "Reservation.hh"
#include "SQLite.hh"
#include "concepts.hh"

Reservation::Reservation(const std::string& id, void* src)
    : src{ src }
    , id{ id }
    , period{ Period{Today, Days{1}} } // TODO: temporary default
  {
    auto* db = static_cast<SQLite*>(src);
    auto stmt = db->prepare(R"(
SELECT room_id, guest_id, begin_date, duration_days
  FROM reservations
 WHERE reservation_id = ?
)");
    std::cout << "Searching for " << id << std::endl;
    stmt.bind(id);

    if (!stmt.next())
      throw std::runtime_error{"Reservation " + id + " not found"};

    room_id = stmt.get<std::string>(0);
    guest_id = stmt.get<std::string>(1);
    period = {
      from_string(stmt.get<std::string>(2)),
      Days{stmt.get<int>(3)}
    };
  }
