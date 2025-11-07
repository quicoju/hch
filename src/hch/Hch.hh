#include <map>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "Hotel.hh"
#include "Repl.hh"
#include "Formatter.hh"

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
    , formatter{}
    , src{ get_source() }
    , hotel{ Hotel{&src} }
  { }

  string prompt() const override
  {
    return current_room.empty()
      ? "hch> "
      : "hch[room " + current_room + "]> ";
  }

  void execute()
  {
    if (conf.use_repl) run();
    else execute(conf.cli_argv.front(), conf.cli_argv);
  }

  void execute(const string& command, const Tokens& tokens) override
  {
    try {
      if (command_table_.contains(command))
        (this->*command_table_.at(command))(tokens);
      else
        std::cout << "---\nError: Unknown command " << command << std::endl;
    }
    catch (const std::exception& e) {
      std::cout << "---\nError: " << e.what() << std::endl;
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
        else if (arg.find("--repl") == 0)
          use_repl = true;
        // keep the command line arguments
        cli_argv.push_back(std::move(arg));
      }
    }
    Tokens cli_argv;
    string db_path = "db/hotel.db";
    bool use_repl = false;
  };


  /*
    extract the first value with the form "name=value" from the tokens
   */
  std::optional<string> value_for(const string& name, const Tokens& tokens)
  {
    for (const auto& t: tokens ) {
      if ( t.find(name) != t.npos) {
        if (auto pos = t.find("="); pos != t.npos)
          return t.substr(pos+1);
      }
    }
    return {};
  }

  Room ensure_room(const string& usage, const Tokens& tokens)
  {
    string room_id{current_room};

    if (room_id.empty())
      room_id = value_for("--room", tokens).value_or("");

    try { return hotel.room(room_id); }
    catch(const std::invalid_argument& e) {
      throw std::invalid_argument{"Command usage: " + usage + " --room=ROOM"};
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
  Formatter formatter;
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
    try { current_room = hotel.room(tokens[1]).id; }
    catch(const std::invalid_argument& e) {
      throw std::invalid_argument{"Command usage: set-room ID"};
    }
  }

  void unset_room(const Tokens& tokens)
  {
    current_room.clear();
  }

  void list_reservations(const Tokens& tokens)
  {
    auto room = ensure_room("list-reservations", tokens);
    auto reservations = hotel.reservations_for(room);
    std::cout << formatter.output(reservations, [](const auto& r) {
      return _pstr(r.period);
    }).str() << std::endl;
  }

  void list_amenities(const Tokens& tokens)
  {
    auto r = ensure_room("list-amenities", tokens);
    std::cout << formatter.output(r.amenities()).str() << std::endl;
  }

  void list_rate(const Tokens& tokens)
  {
    auto room = ensure_room("list-rate [--during=[DATE][+DAYS]]", tokens);
    auto date_str = value_for("--during", tokens).value_or("");
    auto [date, duration] = parse_date(date_str);
    std::cout
      << formatter.output(hotel.rate_report_for(room, date, duration)).str()
      << std::endl;
  }

  void reserve(const Tokens& tokens)
  {
    string usage{"reserve --guest=ID [--during=[DATE][+DAYS]]"};
    auto room = ensure_room(usage, tokens);
    auto guest_opt = value_for("--guest", tokens);
    if (!guest_opt)
      throw std::invalid_argument {"Command usage: " + usage};

    auto [date, duration] = parse_date(value_for("--during", tokens).value_or(""));
    auto id = hotel.reserve(*guest_opt, room, date, duration);
    std::cout << "---\nReservation: " << id << std::endl;
  }

  void cancel_reservation(const Tokens& tokens)
  {
    auto reservation_opt = value_for("--id", tokens);
    if (!reservation_opt) throw std::invalid_argument {
        "Command usage: cancel-reservation --id=RESERVATION-ID"};

    hotel.cancel(*reservation_opt);
    std::cout << "---\nReservation: cancelled.\n";
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
