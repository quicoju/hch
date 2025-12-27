#pragma once

#include <chrono>
#include <functional>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

#include "config.h"

using string_view = std::string_view;

using DateTime = std::chrono::system_clock::time_point;
using Days = std::chrono::days;
using Duration = std::chrono::days;

struct Date : std::chrono::year_month_day
{
  Date()
    : std::chrono::year_month_day{} {}

  Date(const std::chrono::year_month_day& d)
    : std::chrono::year_month_day{d} {}

  Date(const std::string& s)
    : std::chrono::year_month_day{ from_string(s) } {}

  std::string as_string() const
  {
    return std::format("{:04}-{:02}-{:02}",
       int(year()), unsigned(month()), unsigned(day()));
  }

  static Date from_string(const std::string& s)
  {
    int year;
    unsigned int month, day;
    char separator;
    std::istringstream ss{s};
    ss >> year >> separator >> month >> separator >> day;
    return std::chrono::year{year} / std::chrono::month{month} / std::chrono::day{day};
  }

  static Date today()
  {
    return Date{std::chrono::floor<Days>(std::chrono::system_clock::now())};
  }
};

struct Period {
  using as_days = std::chrono::time_point<std::chrono::system_clock, Days>;

  Period(Date s, Days d) : start_(s), duration_(d) { }

  inline Date start() const { return start_; }
  inline Days duration() const { return duration_; }
  inline Date end() const { return Date{as_days{start()} + duration()}; }

  bool intersects(const Period& other) const
  {
    return start() < other.end() && other.start() < end();
  }

  bool operator==(const Period& other) const
  {
    return start() == other.start() && duration() == other.duration();
  }

  // period string
  std::string as_string() const
  {
    return std::format("[{}/{}]", start().as_string(), end().as_string());
  }

private:
  Date start_;
  Days duration_;
};

struct Room;
using Rooms = std::vector<Room>;

struct Reservation;
using Reservations = std::vector<Reservation>;

using Amenity = std::string;
using Amenities = std::set<Amenity>;
static inline Amenity AirConditioning = "AirConditioning";
static inline Amenity Balcony = "Balcony";
static inline Amenity Wifi = "Wifi";
static inline Amenity MiniBar = "MiniBar";

using RateDetails = std::map<std::string, double>;
struct RateReport {
  Date date;
  Duration duration;
  double total;
  RateDetails details;
};


// log utils
// =========
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

using Logger = std::shared_ptr<spdlog::logger>;

struct Log {
#define ADD_LOGGING_FUNCTION(LEVEL) \
  template<typename FmtStr, typename... Args> \
  static inline void \
  LEVEL(Logger& l, FmtStr&& fmt, Args&&... args) { \
    if (l) l->LEVEL(std::forward<FmtStr>(fmt), std::forward<Args>(args)...); \
  }
  ADD_LOGGING_FUNCTION(debug);
  ADD_LOGGING_FUNCTION(info);
  ADD_LOGGING_FUNCTION(warn);
  ADD_LOGGING_FUNCTION(error);

  static Logger logger()
  {
    auto logger = Logger{};
    if (auto level = std::getenv("HCH_LOG_LEVEL")) {
      auto file = std::getenv("HCH_LOG_FILE");
      logger = file
        ? spdlog::basic_logger_mt("hch", file)
        : spdlog::stdout_logger_mt("hch");
      logger->set_level(spdlog::level::from_str(level));
    }
    return logger;
  }
};
