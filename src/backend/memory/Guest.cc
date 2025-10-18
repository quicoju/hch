#include <exception>

#include "Guest.hh"
#include "HotelData.hh"

Guest::Guest(const std::string& id, void* src)
  :src{src}
  ,email{id}
{
  auto* guests = static_cast<GuestsData*>(src);
  for (const auto& guest: *guests) {
    if (guest.id == id) return;
  }

  //throw std::runtime_error{"Guest " + id + " not found"};
}
