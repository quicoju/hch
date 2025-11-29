#include <map>
#include <optional>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "Annotate.hh"
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

  void execute()
  {
    if (conf.use_repl) return run();

    auto& args = conf.cli_argv;
    auto command = args.size() ? args.front() : "help";
    execute(command, args);
  }

  void execute(const string& command, const Tokens& tokens) override
  {
    try {
      if (command_table_.contains(command))
        (this->*command_table_.at(command))(tokens);
      else
        reply_with("Error", "Unknown command "+ command);
    }
    catch (const std::exception& e) {
      reply_with("Error", e.what());
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


  template<typename T>
  void reply_with(const T& data)
  {
    std::cout << "---\n";
    annotate::to(std::cout, data);
    std::cout << std::endl;
  }

  template<typename T>
  void reply_with(std::string_view k, T v)
  {
    std::map<std::string_view, T> data{{k, v}};
    reply_with(data);
  }

  /*
    extract the first value with the form "name=value" from the tokens
   */
  std::optional<string> value_for(const string& name, const Tokens& tokens)
  {
    for (const auto& t: tokens) {
      if (t.find(name) != t.npos) {
        if (auto pos = t.find("="); pos != t.npos)
          return t.substr(pos+1);
      }
    }
    return {};
  }

  string ensure_room(const std::string& usage, const Tokens& tokens)
  {
    string room_id{current_room};

    if (room_id.empty())
      room_id = value_for("--room", tokens).value_or("");

    if (room_id.empty())
      throw std::invalid_argument{"Command usage: " + usage + " --room=ROOM"};

    return room_id;
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
    {"record-guest", &Hch::record_guest},
    {"reserve", &Hch::reserve},
    {"cancel-reservation", &Hch::cancel_reservation},
    {"show-reservation", &Hch::show_reservation},
    {"show-reservation-notes", &Hch::show_reservation_notes},
    {"patch-reservation-notes", &Hch::patch_reservation_notes},
    {"checkin", &Hch::checkin},
    {"checkout", &Hch::checkout},
    {"help", &Hch::help},
    {"quit", &Hch::quit},
    {"exit", &Hch::quit}
  };

  ////////////////////////////
  // Command implementation //
  ////////////////////////////

  void set_room(const Tokens& tokens)
  {
    if (tokens.size() < 2)
      throw std::invalid_argument{"Command usage: set-room ID"};

    try { current_room = hotel.room(tokens[1]).id; }
    catch(const std::invalid_argument& e) {
      throw std::invalid_argument{"this room doesn't exist"};
    }
  }

  void unset_room(const Tokens& tokens)
  {
    current_room.clear();
  }

  void list_reservations(const Tokens& tokens)
  {
    const string usage{
    "list-reservations "
      "[--starting_on=DATE | --ending_on=DATE | --guest=ID | --room=ID]"
    };
    Reservations reservations{};

    if (auto str = value_for("--starting_on", tokens)) {
      reservations = hotel.reservations_starting_on(parse_date(*str).first);
    }
    else if (auto str = value_for("--ending_on", tokens)) {
      reservations = hotel.reservations_ending_on(parse_date(*str).first);
    }
    else if (auto guest = value_for("--guest", tokens)) {
      reservations = hotel.guest_reservations(*guest);
    }
    else {
      reservations = hotel.room_reservations(ensure_room(usage, tokens));
    }
    std::map<std::string, std::string> summary;
    for (const auto& r: reservations)
      summary.insert({r.id, _pstr(r.period)});
    reply_with(summary);
  }

  void list_amenities(const Tokens& tokens)
  {
    auto r = hotel.room(ensure_room("list-amenities", tokens));
    reply_with(r.amenities());
  }

  void list_rate(const Tokens& tokens)
  {
    auto room = ensure_room("list-rate [--during=[DATE][+DAYS]]", tokens);
    auto date_str = value_for("--during", tokens).value_or("");
    auto [date, duration] = parse_date(date_str);
    reply_with(hotel.rate_report_for(room, date, duration));
  }

  void record_guest(const Tokens& tokens)
  {
    auto email = value_for("--email", tokens);
    if (!email) throw std::invalid_argument{
        "Command usage: record-guest --email=EMAIL"};
    reply_with("Guest", hotel.record_guest(*email));
  }

  void reserve(const Tokens& tokens)
  {
    string usage{"reserve --guest=ID [--during=[DATE][+DAYS]]"};
    auto room = hotel.room(ensure_room(usage, tokens));
    auto guest_opt = value_for("--guest", tokens);
    if (!guest_opt)
      throw std::invalid_argument {"Command usage: " + usage};

    auto [date, duration] = parse_date(value_for("--during", tokens).value_or(""));
    auto id = hotel.reserve(*guest_opt, room, date, duration);
    reply_with("Reservation", id);
  }

  void cancel_reservation(const Tokens& tokens)
  {
    auto reservation_opt = value_for("--id", tokens);
    if (!reservation_opt) throw std::invalid_argument {
        "Command usage: cancel-reservation --id=RESERVATION"};
    hotel.cancel(*reservation_opt);
    reply_with("Reservation cancelled");
  }

  void show_reservation(const Tokens& tokens)
  {
    auto id = value_for("--id", tokens);
    if (!id) throw std::invalid_argument {
        "Command usage: show-reservation --id=RESERVATION"};
    reply_with(hotel.reservation(*id));
  }

  void show_reservation_notes(const Tokens& tokens)
  {
    auto id = value_for("--id", tokens);
    if (!id) throw std::invalid_argument {
        "Command usage: show-reservation-notes --id=RESERVATION"};
    // TODO: maybe create a "reply_verbatim_with" function
    // or maybe create a "Notes" type that is a YAML "node".
    std::cout << "---\n" << hotel.reservation_notes(*id) << std::endl;
  }

  void patch_reservation_notes(const Tokens& tokens)
  {
    const string usage {
      "patch-reservation-notes --id=RESERVATION --title=TITLE [--content=CONTENT]"};
    auto id = value_for("--id", tokens);
    auto title = value_for("--title", tokens);
    auto content = value_for("--content", tokens).value_or("");

    if (!id || !title) throw std::invalid_argument {
        "Command usage: " + usage };
    hotel.patch_reservation_notes(*id, *title, content);
    reply_with("Notes updated");
  }

  void checkin(const Tokens& tokens)
  {
    auto reservation_opt = value_for("--id", tokens);
    if (!reservation_opt) throw std::invalid_argument {
      "Command usage: checkin --id=RESERVATION-ID"};
    hotel.checkin(*reservation_opt);
    reply_with("Reservation checked-in");
  }

  void checkout(const Tokens& tokens)
  {
    auto reservation_opt = value_for("--id", tokens);
    if (!reservation_opt) throw std::invalid_argument {
      "Command usage: checkout --id=RESERVATION-ID"};
    hotel.checkout(*reservation_opt);
    reply_with("Reservation checked-out");
  }

  void help(const Tokens& tokens)
  {
    reply_with("Available commands", supported_commands());
  }

  void quit(const Tokens& tokens)
  {
    std::exit(0);
  }
}; // struct Hch
