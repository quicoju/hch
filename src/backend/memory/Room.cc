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
  auto agenda = reservations();
  auto end = agenda.cend();

  return end == std::find_if(agenda.cbegin(), end,
      [&p](Reservation r){ return r.intersects(p); } );
}

void  Room::reserve(Date date, Duration dur)
{
  if (!is_available_on(date, dur))
    throw std::logic_error{"Room is unavailable on the given Date/Duration"};

  agenda().push_back({date, dur});
}

void Room::cancel_reservation(Date date)
{
  const Period p{date, Days{1}};
  auto &agenda_ = agenda();
  auto end = agenda_.end();

  auto match = std::find_if(agenda_.begin(), end,
      [&p](Reservation r){ return r.intersects(p); } );

  if (match != end)
    agenda_.erase(match);
}

const std::list<Reservation>& Room::reservations() const
{
  return *static_cast<std::list<Reservation> *>(src);
}

std::list<Reservation>& Room::agenda() const
{
  return const_cast<std::list<Reservation>&>(reservations());
}
