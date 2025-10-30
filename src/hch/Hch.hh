#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

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

  std::vector<string> supported_commands() const override
  {
    std::vector<string> commands;
    commands.reserve(command_table_.size());
    for (const auto& [cmd, _]: command_table_)
      commands.emplace_back(cmd);
    return commands;
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


  Room ensure_room(const string& usage, const Tokens& tokens)

  {
    string room_id{current_room};

    // if no room is in the context, then expect
    // the room to be the last argument
    if (room_id.empty())
      room_id = tokens.back();

    try { return hotel.room(room_id); }
    catch(const std::invalid_argument& e) {
      throw std::invalid_argument{"Command usage: " + usage + " ROOM"};
    }
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
  std::map<string, Action> command_table_ = {
    {"set-room", &Hch::set_room},
    {"unset-room", &Hch::unset_room},
    {"list-reservations", &Hch::list_reservations},
    {"list-amenities", &Hch::list_amenities},
    {"list-rate", &Hch::list_rate},
    {"reserve", &Hch::reserve},
    {"cancel-reservation", &Hch::cancel_reservation},
    {"help", &Hch::help},
    {"quit", &Hch::quit},
    {"exit", &Hch::quit}
  };

  ////////////////////////////
  // Command implementation //
  ////////////////////////////

  void set_room(const Tokens& tokens)
  {
    current_room.clear();
    current_room = ensure_room("set-room", tokens).id;
  }

  void unset_room(const Tokens& tokens)
  {
    current_room.clear();
  }

  void list_reservations(const Tokens& tokens)
  {
    auto room = ensure_room("list-reservations", tokens);
    for (const auto& r : hotel.reservations_for(room))
      std::cout << "  - " << _pstr(r.period) << std::endl;
  }

  void list_amenities(const Tokens& tokens)
  {
    auto r = ensure_room("list-amenities", tokens);
    for (const auto& a : r.amenities())
      std::cout << "  - " << a << std::endl;
  }

  void list_rate(const Tokens& tokens)
  {
    auto room = ensure_room("list-rate [DATE[+DAYS]]", tokens);
    auto date_str = tokens.size() > 2 ? tokens[1] : "";
    auto [date, duration] = parse_date(date_str);

    auto report = hotel.rate_report_for(room, date, duration);
    for(auto& [name, cost]: report) {
      if (name == "Total") continue;
      std::cout << "  - " << name << ": " << cost << std::endl;
    }
    std::cout << "  - Total: " << report["Total"] << std::endl;
  }

  void reserve(const Tokens& tokens)
  {
    auto room = ensure_room("reserve GUEST-ID [DATE[+DAYS]]", tokens);
    if (tokens.size() < 2) throw std::invalid_argument {
        "Command usage: reserve GUEST-ID [DATE[+DAYS]]"};

    string guest_id{tokens[1]};
    string date_str{tokens.size()>=3 ? tokens[2]: ""};

    auto [date, duration] = parse_date(date_str);
    auto id = hotel.reserve(guest_id, room, date, duration);
    std::cout << "Reservation " << id << " successfully created\n";
  }

  void cancel_reservation(const Tokens& tokens)
  {
    if (tokens.size() < 2) throw std::invalid_argument {
        "Command usage: cancel-reservation RESERVATION-ID"};

    hotel.cancel(tokens[1]);
    std::cout << "Reservation cancelled.\n";
  }

  void help(const Tokens& tokens)
  {
    std::cout << "Available commands:\n";
    for (const auto& [command, _]: command_table_)
      std::cout << "  - "<< command << std::endl;
  }

  void quit(const Tokens& tokens)
  {
    std::exit(0);
  }
}; // struct Hch
