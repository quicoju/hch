#include <exception>

#include "Guest.hh"
#include "SQLite.hh"

std::string
Guest::record(std::string_view email, Backend* db)
{
  // TODO: Let's not make an error if the guest already exists,
  // but this may change in the future, so for now we search
  // by "ID" but later it'll have to search by "E-mail"
  try {
    return find_by_id(email, db).id;
  }
  catch (const std::runtime_error& e) {
    if (!std::string{e.what()}.ends_with("not found"))
      throw e;
  }

  Guest g{email, db};
  db->prepare(R"(
INSERT INTO guests(email) VALUES(?)
)").execute(g.id);

  return g.id;
}

Guest
Guest::find_by_id(std::string_view id, Backend* db)
{
  auto stmt = db->prepare(R"(
SELECT email
 FROM guests
WHERE email = ?
)");
  stmt.bind(id);
  if (!stmt.next())
    throw std::runtime_error{std::format("Guest {} not found", id)};

  return Guest{stmt.get<std::string>(), db};
}
