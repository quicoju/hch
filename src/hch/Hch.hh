#include <regex>
#include <stdexcept>

#include "Hotel.hh"
#include "Repl.hh"

#ifdef USE_sqlite
#include "SQLite.hh"
using Backend = SQLite;
#else
#include "HotelData.hh"
using Backend = HotelData;
#endif

struct Hch : Repl {
  Hch(int argc, char* argv[])
    : conf{ argc, argv }
    , src{ get_source() }
    , hotel{ Hotel{&src} }
  { }

  string prompt() const override
  {
    return current_room.empty()
      ? "hch> "
      : "hch[room " + current_room + "]> ";
  }

  void execute(const string& command, const Tokens& tokens) override
  {
    try {
      if (command_table_.contains(command))
        (this->*command_table_.at(command))(tokens);
      else
        std::cout << "Unknown command: " << command << std::endl;
    }
    catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << std::endl;
    }
  }

private:
  struct Config {
    Config(int argc, char *argv[]) {
      for (auto i{1}; i < argc; i++) {
        string arg{argv[i]};
        if (arg.find("--db=") == 0)
          db_path = arg.substr(5);
      }
    }
    string db_path = "db/hotel.db";
  };


  Room ensure_room(const string& command, const Tokens& tokens)
  {
    string room_id{current_room};

    if (room_id.empty() && tokens.size() >= 2)
      room_id = tokens[1];

    if (room_id.empty()) throw std::invalid_argument{
        string{"Command usage: "} + command + " ROOM"};

    return hotel.room(room_id);
  }

  std::pair<Date, Duration> parse_date(const string &s)
  {
    using namespace std;

    // provide defaults
    if (s.empty()) return {Today, Days{1}};

    // if no date is provided but a duration, then make it
    // relative to today's date i.e. +3d
    if (s.front() == '+') {
      regex re(R"(\+(\d+)d)");
      smatch match;
      if (!regex_match(s, match, re))
        throw std::invalid_argument{"Invalid duration format"};
      return {Today, Days{stoi(match[1])}};
    }

    // Parse date string with optional duration, i.e.
    // "2025-11-03" or "2025-11-03+3d"
    regex re(R"((\d{4}-\d{2}-\d{2})(?:\+(\d+)d)?)");
    smatch match;

    if (!regex_match(s, match, re))
      throw std::invalid_argument{"Invalid date format"};

    Date date{from_string(match[1])};
    Duration duration{match[2].matched ? stoi(match[2]) : 1};

    return {date, duration};
  }

  Backend get_source()
  {
    return Backend{conf.db_path};
  }

  /* State */
  Config conf;
  Backend src;
  Hotel hotel;
  string current_room;

  using Action = void(Hch::*)(const Tokens&);
  std::unordered_map<string, Action> command_table_ = {
    {"set-room", &Hch::set_room},
    {"unset-room", &Hch::unset_room},
    {"list-reservations", &Hch::list_reservations},
    {"list-amenities", &Hch::list_amenities},
    {"list-rate", &Hch::list_rate},
    {"reserve", &Hch::reserve},
    {"cancel-reservation", &Hch::cancel_reservation},
    {"quit", &Hch::quit},
    {"exit", &Hch::quit}
  };

  ////////////////////////////
  // Command implementation //
  ////////////////////////////

  void set_room(const Tokens& tokens)
  {
    if (tokens.size() < 2) throw std::invalid_argument{
        "Command usage: set-room ROOM"};
    current_room = hotel.room(tokens[1]).id;
  }

  void unset_room(const Tokens& tokens)
  {
    current_room.clear();
  }

  void list_reservations(const Tokens& tokens)
  {
    auto r = ensure_room("list-reservations", tokens);
    for (const auto& reservation : r.reservations())
      std::cout << "  - " << _pstr(reservation) << std::endl;
  }

  void list_amenities(const Tokens& tokens)
  {
    auto r = ensure_room("list-amenities", tokens);
    for (const auto& a : r.amenities())
      std::cout << "  - " << a << std::endl;
  }

  void list_rate(const Tokens& tokens)
  {
    auto room = ensure_room("list-rate", tokens);
    auto n_arg = current_room.empty() ? 2U : 1;
    auto date_str = tokens.size() > n_arg ? tokens[n_arg] : "";
    auto [date, duration] = parse_date(date_str);

    auto report = hotel.rate_report_for(room, date, duration);
    for(auto& [name, cost]: report)
      std::cout << " - " << name << ": " << cost << std::endl;
  }

  void reserve(const Tokens& tokens)
  {
    if (tokens.size() < 3) throw std::invalid_argument {
        "Command usage: reserve GUEST-ID DATE[+DAYS] [ROOM]"};

    string guest_id{tokens[1]};
    string date_str{tokens[2]};
    string room_id{current_room};

    if (room_id.empty()) {
      if (tokens.size() < 4) throw std::invalid_argument {
          "Command usage: reserve GUEST-ID DATE[+DAYS] ROOM" };
      else room_id = tokens[3];
    }

    auto [date, duration] = parse_date(date_str);
    auto room = hotel.room(room_id);
    auto id = hotel.reserve(guest_id, room, date, duration);
    std::cout << "Reservation " << id << " successfully created\n";
  }

  void cancel_reservation(const Tokens& tokens)
  {
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

  void quit(const Tokens& tokens)
  {
    std::exit(0);
  }
}; // struct Hch
