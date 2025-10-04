#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

#include "Hotel.hh"

#include "backend/memory/HotelData.hh"
#ifdef USE_sqlite
#include "backend/sqlite/SQLite.hh"
using Backend = SQLite;
#else
using Backend = HotelData;
struct SQLite; // mock unused backend
#endif

struct Repl {
  using string = std::string;
  using Tokens = std::vector<string>;

  virtual string prompt() const = 0;
  virtual void execute(const string&, const Tokens&) = 0;

  void run() {
    while (true) {
      char *line = readline(prompt().c_str());
      if (!line) break;              /* EOF */
      if (*line) add_history(line);  /* don't keep empty lines */
      execute_command(tokenize(line));
      free(line);
    }
  }

private:
  Tokens tokenize(const string &line) {
    std::istringstream iss(line);
    string token;
    Tokens tokens;

    while (iss >> token)
      tokens.push_back(token);

    return tokens;
  }

  void execute_command(const Tokens& t) {
    if (t.empty()) return;
    string command = t[0];
    execute(command, t);
  }
};

template<typename T=Backend>
struct Hch : Repl {
  Hch(int argc, char* argv[])
    : conf{ argc, argv }
    , src{ get_source() }
    , hotel{ Hotel{&src} }
  { }

  string prompt() const override {
    return current_room.empty()
      ? "(hch) "
      : "(hch " + current_room + ") ";
  }

  void execute(const string& command, const Tokens& tokens) override {
    try {
      if (command == "set-room") {
        if (tokens.size() < 2) throw std::invalid_argument{
            "Command usage: set-room ROOM"};
        current_room = hotel.room(tokens[1]).id;
      }
      else if (command == "unset-room") {
        current_room.clear();
      }
      else if (command == "list-reservations") {
        auto r = ensure_room(command, tokens);
        for (const auto& reservation : r.reservations())
          std::cout << "  - " << _pstr(reservation) << std::endl;
      }
      else if (command == "list-amenities") {
        auto r = ensure_room(command, tokens);
        for (const auto& a : r.amenities())
          std::cout << "  - " << a << std::endl;
      }
      else if (command == "reserve") {
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
      }
      else if (command == "cancel-reservation") {
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
      }
      else if (command == "quit" || command == "exit") {
        std::exit(0);
      }
      else {
        std::cout << "Unknown command: " << command << std::endl;
      }
    }
    catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << std::endl;
    }
  }
  
private:
  struct Config {
    Config(int argc, char *argv[]) {
      for (auto i{1}; i < argc; i++) {
        std::string arg{argv[i]};
        if (arg.find("--db=") == 0)
          db_path = arg.substr(5);
      }
    }
    string db_path = "db/hotel.db";
  };


  Room ensure_room(const string& command, const Tokens& tokens) {
    string room_id{current_room};

    if (room_id.empty() && tokens.size() >= 2)
      room_id = tokens[1];

    if (room_id.empty()) throw std::invalid_argument{
        string{"Command usage: "} + command + " ROOM"};

    return hotel.room(room_id);
  }

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

  auto get_source() {
    using namespace Amenity;
    if constexpr (std::is_same_v<T, SQLite>) {
      return SQLite{conf.db_path};
    }
    else if constexpr (std::is_same_v<T, HotelData>)  {
      return HotelData{
        {"101", 1, {Wifi}},
        {"102", 1, {Balcony, Wifi}, },
        {"103", 1, {MiniBar, Wifi}, },
        {"201", 1, {AirConditioning, Balcony}, },
        {"202", 1, {Wifi}, },
        {"203", 1, {MiniBar}, },
      };
    }
  }

  /* State */
  Config conf;
  Backend src;
  Hotel hotel;
  string current_room;
}; // struct Hch 


int main (int argc, char *argv[])
{
  Hch<Backend>{argc, argv}.run();

  std::cout << "bye!\n";
  return 0;
}
