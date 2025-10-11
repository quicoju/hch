#include <exception>

#include "Guest.hh"
#include "SQLite.hh"

Guest::Guest(const std::string& id, void* src)
  :src{src}
  ,email{id}
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
}
