#include "Room.hh"

#include <exception>

Room::Room(const std::string id, void *data_source)
 : id{id}
 , src{data_source} {
  // the data_source is a collection of Reservations
}

Room::Room(const char *id, void *data_source)
 : id{id}
 , src{data_source} {
  // the data_source is a collection of Reservations
}


bool Room::is_available_on(Date date, Duration dur) const
{
  const Period p{date, dur};
  auto &agenda = *static_cast<std::list<Reservation> *>(src);;
  auto end = agenda.cend();

  return end == std::find_if(agenda.cbegin(), end,
      [&p](Reservation r){ return r.intersects(p); } );
}

void  Room::reserve(Date date, Duration dur)
{
  if (!is_available_on(date, dur))
    throw std::runtime_error{"Room is already reserved for overlapping dates"};

  auto& agenda = *static_cast<std::list<Reservation> *>(src);
  agenda.push_back({date, dur});
}

void Room::cancel_reservation(Date date)
{
  const Period p{date, Days{1}};
  auto &agenda_ = *static_cast<std::list<Reservation> *>(src);
  auto end = agenda_.end();

  auto match = std::find_if(agenda_.begin(), end,
      [&p](Reservation r){ return r.intersects(p); } );

  if (match != end)
    agenda_.erase(match);
}

const std::list<Reservation> Room::reservations() const
{
  std::list<Reservation> l{};
  auto &_src = *static_cast<std::list<Reservation> *>(src);

  for (const auto r: _src)
    l.push_back(r);

  return l;
}

