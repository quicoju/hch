#pragma once

std::list<Reservation> build_agenda() {
  std::list agenda{
    Reservation{ {2024, 12, 19}, Days{3} },
    Reservation{ {2024, 12, 25}, Days{5} },
  };
  return agenda;
}

Rooms build_one_room_src() {
  return Rooms{{"A-102", {}}};
}

Rooms build_src(Rooms r) {
  return r;
}
