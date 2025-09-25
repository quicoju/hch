#include <iostream>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

#include "Hotel.hh"

// Parse date string like "2025-11-03" or "2025-11-03+3d"
std::pair<Date, Duration> parse_date(const std::string &s)
{
  using namespace std;
  regex re(R"((\d{4}-\d{2}-\d{2})(?:\+(\d+)d)?)");
  smatch match;

  if (!regex_match(s, match, re))
    throw std::invalid_argument{"Invalid date format"};

  Date date{from_string(match[1])};
  Duration duration{match[2].matched ? stoi(match[2]) : 1};

  return {date, duration};
}

struct Repl {
  Repl(Hotel& h) : hotel{h} {}

  void run() {
    while (true) {
      char *line = readline(get_prompt().c_str());
      if (!line) break;              /* EOF */
      if (*line) add_history(line);  /* don't keep empty lines */
      execute_command(tokenize(line));
      free(line);
    }
  }

private:

  using string = std::string;
  using Tokens = std::vector<string>;

  Hotel& hotel;
  string current_room;

  string get_prompt() const {
    return current_room.empty()
      ? "(hch) "
      : "(hch " + current_room + ") ";
  }

  Tokens tokenize(const string &line) {
    std::istringstream iss(line);
    string token;
    Tokens tokens;

    while (iss >> token)
      tokens.push_back(token);

    return tokens;
  }

  void execute_command(const Tokens &tokens) {
    if (tokens.empty()) return;

    const string &command = tokens[0];

    try {
      if (command == "set-room") {
        if (tokens.size() < 2) throw std::invalid_argument{
            "Command usage: set-room ROOM"};
        current_room = hotel.room(tokens[1]).id;
        return;
      }

      if (command == "unset-room") {
        current_room.clear();
        return;
      }

      if (command == "list-reservations") {
        string room_id{current_room};

        if (room_id.empty() && tokens.size() >= 2)
          room_id = tokens[1];

        if (room_id.empty()) throw std::invalid_argument{
            "Command usage: list-reservations ROOM"};

        auto r = hotel.room(room_id);
        for (const auto& reservation : r.reservations())
          std::cout << "  - " << _pstr(reservation) << std::endl;

        return;
      }

      if (command == "reserve") {
        if (tokens.size() < 2) throw std::invalid_argument {
          "Command usage: reserve DATE[+DAYS] [ROOM]"};

        string date_str{tokens[1]};
        string room_id{current_room};

        if (room_id.empty()) {
          if (tokens.size() < 3) throw std::invalid_argument {
              "Command usage: reserve DATE[+DAYS] ROOM" };
          else room_id = tokens[2];
        }

        auto [date, duration] = parse_date(date_str);
        hotel.room(room_id).reserve(date, duration);
        std::cout << "Room reserved successfully.\n";
        return;
      }

      if (command == "cancel-reservation") {
        if (tokens.size() < 2) throw std::invalid_argument {
          "Command usage: cancel-reservation DATE[+DAYS] [ROOM]"};

        string date_str{tokens[1]};
        string room_id{current_room};

        if (room_id.empty()) {
          if (tokens.size() < 3) throw std::invalid_argument {
              "Command usage: cancel-reservation DATE[+DAYS] ROOM" };
          else room_id = tokens[2];
        }

        auto [date, _] = parse_date(date_str);
        hotel.room(room_id).cancel_reservation(date);
        std::cout << "Reservation cancelled.\n";
        return;
      }

      if (command == "quit" || command == "exit")
        std::exit(0);

      std::cout << "Unknown command: " << command << std::endl;
    }
    catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << std::endl;
    }
  }
};

#include <sqlite3.h>

int main (int argc, char *argv[])
{
  // TODO: For now initialize a Hotel here
  void *src;
#ifdef USE_sqlite
  sqlite3* db;
  sqlite3_open("db/hotel.db", &db);
  const char* sql = R"(
DELETE FROM reservations;
INSERT OR IGNORE INTO rooms(id) VALUES
  ('room-101'), ('room-102'), ('room-103'),
  ('room-201'), ('room-202'), ('room-203')
)";
  int result = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error{sqlite3_errmsg(db)};
  }
  src = db;
#else
  using Reservations = std::list<Reservation>;
  std::vector<Reservations> r{6};
  Rooms rooms{
    {"room-101", &r[0]},
    {"room-102", &r[1]},
    {"room-103", &r[2]},
    {"room-201", &r[3]},
    {"room-202", &r[4]},
    {"room-203", &r[5]},
  };
  src = &rooms;
#endif

  Hotel hotel{src};
  Repl repl{hotel};
  repl.run();

  std::cout << "bye!\n";
  return 0;
}
