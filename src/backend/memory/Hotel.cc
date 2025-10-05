#include "Hotel.hh"
#include "HotelData.hh"

Hotel::Hotel(void *data_source)
  : src{data_source} // src is expected to point to a HotelData object
{};

bool Hotel::is_available_on(Date date, Duration dur, size_t n_rooms)
  const noexcept {

  for (const auto &r: rooms()) {
    if (r.is_available_on(date, dur)) {
      --n_rooms;
      if (!n_rooms) return true;
    }
  }
  return false;
}

Rooms Hotel::find_available_on(Date d, Duration dur)
  const noexcept {
  Rooms available_rooms{};

  for (const auto &r: rooms()) {
    if (r.is_available_on(d, dur))
      available_rooms.emplace_back(r);
  }
  return available_rooms;
}

Room Hotel::room(const std::string id) {
  for (auto &r : rooms())
    if (r.id == id) return r;
  throw std::invalid_argument{std::string{"Room "} + id + " not found"};
}

Rooms Hotel::rooms() const {
  auto hotel_data =  static_cast<HotelData *>(src);
  Rooms rooms{};

  for (auto& d: hotel_data->rooms) {
    Room r{d.id, d.capacity, &d};
    rooms.emplace_back(r);
  }

  return rooms;
}
