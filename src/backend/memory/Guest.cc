#include <exception>

#include "Guest.hh"

std::string
Guest::record(std::string_view email, Backend* src)
{
  auto& guests = src->guests;
  auto it = std::ranges::find_if(guests,
      [&](const auto& g) { return g.id == email; });

  // TODO: Let's not make an error if the guest already exists,
  // but this may change in the future
  if (it != guests.end()) return it->id;

  // TODO: still to determine a good identifier for the guest,
  // currently sticking to the email address
  Guest g{email, src};
  guests.emplace_back(g.id);
  return g.id;
}

Guest
Guest::find_by_id(std::string_view id, Backend* src)
{
  for (const auto& g: src->guests)
    if (g.id == id) return Guest{g.id, src};
  throw std::runtime_error{std::format("Guest {} not found", id)};
}
