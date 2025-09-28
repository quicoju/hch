#pragma once

#include "../src/backend/sqlite/SQLite.hh"

static auto test_db = "db/hotel.db";

SQLite build_agenda();
SQLite build_src();
SQLite build_one_room_src();

SQLite build_agenda() {
  SQLite db{test_db};
  db.execute(R"(
DELETE FROM reservations;
INSERT OR IGNORE INTO rooms(name) VALUES ('101');
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days)
VALUES
  ((SELECT id FROM rooms WHERE name = '101'), '2024-12-19', 3),
  ((SELECT id FROM rooms WHERE name = '101'), '2024-12-25', 5);
)");
  return db;
}

SQLite build_src(Rooms r) {
  // TODO: use the Rooms parameter to insert
  // the data into the tables
  SQLite db{SQLite{test_db}};
  db.execute(R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT OR IGNORE INTO rooms(name) VALUES
 ('101'), ('102'), ('103');
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days)
VALUES
  ((SELECT id FROM rooms WHERE name = '101'), '2024-12-19', 3),
  ((SELECT id FROM rooms WHERE name = '102'), '2024-12-20', 1),
  ((SELECT id FROM rooms WHERE name = '103'), '2024-12-31', 4);
)");
  return db;
}

SQLite build_one_room_src() {
  SQLite db{test_db};
  db.execute(R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT INTO rooms(name) VALUES('A-102'))");
  return db;
}

