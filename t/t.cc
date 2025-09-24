#include "../src/Hotel.hh"

#include <sqlite3.h>
#include <tuple>
#include <catch2/catch_test_macros.hpp>

std::list agenda{
  Reservation{ {2024, 12, 19}, Days{3} },
  Reservation{ {2024, 12, 25}, Days{5} },
};

void* build_agenda();
void* build_src();
void* build_one_room_src();
void close_src(void *);

/* Room Tests
 * ==========
 */
TEST_CASE("Room::is_available_on") {
  auto src = build_agenda();
  Room room{ "101", src };
  SECTION("available") {
    Date date{2024, 12, 23};
    REQUIRE(room.is_available_on({2024, 12, 22}));
    REQUIRE(room.is_available_on(date));
    REQUIRE(room.is_available_on(date, Days{2}));
    REQUIRE(room.is_available_on({2024, 12, 23}, Days{2}));
  }
  SECTION("unavailable") {
    REQUIRE(!room.is_available_on({2024, 12, 26}));
    REQUIRE(!room.is_available_on({2024, 12, 23}, Days{3}));
  }
  close_src(src);
}

TEST_CASE("Room::reserve") {
  auto src = build_agenda();
  Room room{ "101", src };
  SECTION("success") {
    Date date{2024, 11, 10};
    room.reserve(date);
    REQUIRE_FALSE(room.is_available_on(date));

    room.reserve({2024, 11, 12}, Days{2});
    REQUIRE_FALSE(room.is_available_on({2024, 11, 12}));
  }
  SECTION("failed") {
    REQUIRE_THROWS_AS(room.reserve({2024, 12, 20}), std::logic_error);
  }
  close_src(src);
}

TEST_CASE("Room::cancel_reservation") {
  auto src = build_agenda();
  Room room{ "101", src };
  SECTION("success") {
    room.cancel_reservation({2024, 12, 20});
    REQUIRE(room.is_available_on({2024, 12, 21}));
  }
  SECTION("No reservation match") {
    room.cancel_reservation({2024, 12, 12});
    REQUIRE(!room.is_available_on({2024, 12, 25}));
  }
  close_src(src);
}

TEST_CASE("Room::reservations") {
  auto src = build_agenda();
  Room room{ "101", src };
  REQUIRE(room.reservations() == agenda);
  close_src(src);
}

/* Hotel Tests
 * ===========
 */
using Reservations = std::list<Reservation>;

TEST_CASE("Hotel::is_available_on") {
  void *src;
#ifdef USE_sqlite
  src = build_src();
#else
  Reservations r_101{ {{2024, 12, 19}, Days{3} }};
  Reservations r_102{ {{2024, 12, 20}, Days{1} }};
  Reservations r_103{ {{2024, 12, 31}, Days{4} }};
  Rooms rooms{
    {"101", &r_101},
    {"102", &r_102},
    {"103", &r_103},
  };
  src = &rooms;
#endif
  Hotel hotel{src};
  REQUIRE(hotel.rooms().size() == 3);

  SECTION("Hotel::is_available_on") {
    Date date{2025, 01, 02};
    REQUIRE_FALSE(hotel.is_available_on(date, Days{2}, 3));
    REQUIRE(hotel.is_available_on(date, Days{2}, 2));
    REQUIRE(hotel.is_available_on(date));
  }
  close_src(src);
}

TEST_CASE("Hotel::find_available_on") {
  void *src;
#ifdef USE_sqlite
  src = build_src();
#else
  Reservations r_101{ {{2024, 12, 19}, Days{3} }};
  Reservations r_102{ {{2024, 12, 20}, Days{1} }};
  Reservations r_103{ {{2024, 12, 31}, Days{4} }};
  Rooms rooms{
    {"101", &r_101},
    {"102", &r_102},
    {"103", &r_103},
  };
  src = &rooms;
#endif
  Hotel hotel{src};
  SECTION("available") {
    Date date{2024, 12, 19};
    auto available = hotel.find_available_on(date, Days{3});
    REQUIRE(available.size() == 1);
    REQUIRE(available[0].id == "103");
  }

  SECTION("unavailable") {
    Date date{2024, 12, 1};
    REQUIRE(hotel.find_available_on(date, Days{40}).empty());
  }
  close_src(src);
}

TEST_CASE("Hotel::room") {
  void *src;
#ifdef USE_sqlite
  src = build_one_room_src();
#else
  Reservations r{};
  Rooms rooms{{"A-102", &r}};
  src = &rooms;
#endif
  Hotel hotel{src};
  SECTION("existing") {
    REQUIRE(hotel.room("A-102").id == "A-102");
  }
  SECTION("non-existing") {
    REQUIRE_THROWS_AS(hotel.room("102"), std::invalid_argument);
  }
  close_src(src);
}

void* build_agenda() {
#ifdef USE_sqlite
  sqlite3* db;
  sqlite3_open("db/hotel.db", &db);
  const char* sql = R"(
DELETE FROM reservations;
INSERT OR IGNORE INTO rooms(id) VALUES ('101');
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days) VALUES
  ('101', '2024-12-19', 3),
  ('101', '2024-12-25', 5);
)";

  int result = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error{sqlite3_errmsg(db)};
  }

  return db;
#else
  return &agenda;
#endif
}

void* build_src() {
  void *src = nullptr;
#ifdef USE_sqlite
  sqlite3* db;
  sqlite3_open("db/hotel.db", &db);
  const char* sql = R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT OR IGNORE INTO rooms(id) VALUES
 ('101'), ('102'), ('103');
INSERT OR IGNORE INTO reservations(room_id, begin_date, duration_days) VALUES
  ('101', '2024-12-19', 3),
  ('102', '2024-12-20', 1),
  ('103', '2024-12-31', 4);
)";

  int result = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error{sqlite3_errmsg(db)};
  }
  src = db;
#endif
  return src;
}

void* build_one_room_src() {
  void *src = nullptr;
#ifdef USE_sqlite
  sqlite3* db;
  sqlite3_open("db/hotel.db", &db);
  const char* sql = R"(
DELETE FROM reservations;
DELETE FROM rooms;
INSERT INTO rooms(id) VALUES ('A-102');
)";

  int result = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error{sqlite3_errmsg(db)};
  }
  src = db;
#endif
  return src;
}

void close_src(void *db) {
#ifdef USE_sqlite
  sqlite3_close(static_cast<sqlite3*>(db));
#endif    
}
