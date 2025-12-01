#pragma once

#include <utility>

#include "SQLite.hh"

inline auto conn_str(){ return "db/unit_test.db"; }

void prepare_tests()
{
  SQLite db{conn_str()};
  db.read_file("db/schema.sql");
  db.read_file("db/mockhotel.sql");
}

SQLite build_src()
{
  SQLite db{conn_str()};
  db.execute(R"(DELETE FROM reservations;)");
  return db;
}

SQLite build_src_with_reservations() {
  auto db = build_src();
  db.execute(R"(
INSERT INTO
reservations(id, guest_id, reservation_id, room_id, begin_date, duration_days)
VALUES
  (1, 1, "A-001", (SELECT id FROM rooms WHERE name = '101'), '2024-12-19', 3),
  (2, 1, "A-002", (SELECT id FROM rooms WHERE name = '102'), '2024-12-20', 1),
  (3, 1, "A-003", (SELECT id FROM rooms WHERE name = '103'), '2024-12-31', 4);
)");
  return db;
}
