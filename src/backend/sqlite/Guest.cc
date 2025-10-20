#include <exception>

#include "Guest.hh"
#include "SQLite.hh"

Guest
Guest::find_by_id(const std::string& id, void* src)
{
  auto* db = static_cast<SQLite*>(src);
  auto stmt = db->prepare(R"(
SELECT email
 FROM guests
WHERE email = ?
)");
  stmt.bind(id);
  if (!stmt.next())
    throw std::runtime_error{"Guest " + id + " not found"};

  return Guest{stmt.get<std::string>(), db};
}
