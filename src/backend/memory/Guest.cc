#include <exception>

#include "Guest.hh"
#include "HotelData.hh"

Guest
Guest::find_by_id(const std::string& id, void* src)
{
  const auto& guests = static_cast<HotelData*>(src)->guests;
  for (const auto& g: guests)
    if (g.id == id) return Guest{g.id, src};
  throw std::runtime_error{"Guest " + id + " not found"};
}
