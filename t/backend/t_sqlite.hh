#pragma once

#include "SQLite.hh"

static auto test_db = "db/unit_test.db";

void prepare_tests()
{
  SQLite db{test_db};
  db.read_file("db/schema.sql");
  db.read_file("db/mockhotel.sql");
}

SQLite build_src() {
  SQLite db{SQLite{test_db}};
  db.execute(R"(
DELETE FROM guests;
DELETE FROM rate_types;
DELETE FROM rates;
DELETE FROM reservations;
DELETE FROM rooms;
INSERT INTO
rooms(id, name, capacity) VALUES
 (1, '101', 1),
 (2, '102', 1),
 (3, '103', 3),
 (7, '301', 1),
 (8, '302', 3);
INSERT INTO
guests (id, email) VALUES
  (1, 'juan.camaney@aol.com');
INSERT INTO
reservations(id, guest_id, reservation_id, room_id, begin_date, duration_days)
VALUES
  (1, 1, "A-001", (SELECT id FROM rooms WHERE name = '101'), '2024-12-19', 3),
  (2, 1, "A-002", (SELECT id FROM rooms WHERE name = '102'), '2024-12-20', 1),
  (3, 1, "A-003", (SELECT id FROM rooms WHERE name = '103'), '2024-12-31', 4);
INSERT OR IGNORE INTO
rate_types (id, name) VALUES
  (1, 'Base'),
  (2, 'Capacity'),
  (3, 'Amenity');
INSERT OR IGNORE
INTO rates (type_id, key_name, value) VALUES
  (1, '',        58.99), -- default base rate
  (1, '101',    100.99), -- premium room rate
  (2, '',         0.20), -- default capacity surcharge
  (3, 'Wifi',     5.00), -- wifi amenity
  (3, 'Balcony', 15.00); -- balcony amenity
)");
  return db;
}

SQLite some_guests(const std::vector<std::string>& guests) {
  SQLite db{test_db};
  auto ix = 1;
  auto stmt = db.prepare(R"(
INSERT OR IGNORE INTO guests (id, email) VALUES(?, ?)
)");
  for (const auto& guest: guests)
    stmt.execute(ix++, guest);
  return db;
}

// Rate tests
SQLite build_rate_table() {
  SQLite db{test_db};
  db.execute(R"(
INSERT OR IGNORE INTO rate_types (id, name)
VALUES
    (1, 'Base'),
    (2, 'Capacity'),
    (3, 'Amenity');

INSERT OR IGNORE INTO rates (type_id, key_name, value)
VALUES
    (1, '',        58.99), -- default base rate
    (1, '101',    100.99), -- premium room rate
    (2, '',         0.20), -- default capacity surcharge
    (3, 'Wifi',     5.00), -- wifi amenity
    (3, 'Balcony', 15.00); -- balcony amenity)");

  return db;
}
