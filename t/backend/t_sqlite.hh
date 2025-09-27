#pragma once

#include "../src/backend/sqlite/SQLite.hh"

SQLite build_agenda();
SQLite build_src();
SQLite build_one_room_src();

SQLite build_agenda() {
  SQLite db{"db/hotel.db"};
  db.execute(R"(
DELETE FROM reservations;
INSERT OR IGNORE INTO rooms(id) VALUES ('101');
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days)
VALUES
  ('101', '2024-12-19', 3),
  ('101', '2024-12-25', 5);
)");
  return db;
}

SQLite build_src(Rooms r) {
  // TODO: use the Rooms parameter to insert
  // the data into the tables
  SQLite db{SQLite{"db/hotel.db"}};
  db.execute(R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT OR IGNORE INTO rooms(id) VALUES
 ('101'), ('102'), ('103');
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days)
VALUES
  ('101', '2024-12-19', 3),
  ('102', '2024-12-20', 1),
  ('103', '2024-12-31', 4);
)");
  return db;
}

SQLite build_one_room_src() {
  SQLite db{"db/hotel.db"};
  db.execute(R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT INTO rooms(id) VALUES('A-102'))");
  return db;
}

