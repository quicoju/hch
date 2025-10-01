#pragma once

#include "../src/backend/sqlite/SQLite.hh"

static auto test_db = "db/hotel.db";

SQLite build_agenda() {
  SQLite db{test_db};
  db.execute(R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT INTO rooms(id, name, capacity)
VALUES
 (1, '101', 1),
 (2, '102', 1),
 (3, '103', 1),
 (4, '201', 1),
 (5, '202', 1),
 (6, '203', 1);
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days)
VALUES
  ((SELECT id FROM rooms WHERE name = '101'), '2024-12-19', 3),
  ((SELECT id FROM rooms WHERE name = '101'), '2024-12-25', 5);
)");
  return db;
}

SQLite build_src() {
  SQLite db{SQLite{test_db}};
  db.execute(R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT INTO rooms(id, name, capacity) VALUES
 (1, '101', 1),
 (2, '102', 1),
 (3, '103', 1);
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
INSERT INTO rooms(id, name, capacity) VALUES(1, 'A-102', 1))");
  return db;
}

