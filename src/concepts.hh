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
using Amount = double;

// Date utils
// ==========
/**
 * @brief Type to keep date and time information
 */
using DateTime = std::chrono::system_clock::time_point;

/**
 * @brief Representation the quantity of days
 */
using Days = std::chrono::days;

/**
 * @brief Type to represent a date w/o a time
 */
struct Date : std::chrono::year_month_day
{
  Date()
    : std::chrono::year_month_day{} {}

  Date(const std::chrono::year_month_day& d)
    : std::chrono::year_month_day{d} {}

  /**
   * @brief Build a Date from a string
   * @param A date string with the format "YYYY-MM-DD"
   */
  Date(const std::string& s)
    : std::chrono::year_month_day{ from_string(s) } {}

  /**
   * @brief "Stringify" a date
   * @return A date string with the format "YYYY-MM-DD"
   */
  inline std::string as_string() const
  {
    return std::format("{:04}-{:02}-{:02}",
       int(year()), unsigned(month()), unsigned(day()));
  }

  // See the constructors for the documentation
  static Date from_string(const std::string& s)
  {
    int year;
    unsigned int month, day;
    char separator;
    std::istringstream ss{s};
    ss >> year >> separator >> month >> separator >> day;
    return std::chrono::year{year} / std::chrono::month{month} / std::chrono::day{day};
  }

  /**
   * @brief Today's date
   */
  static inline Date today()
  {
    return Date{std::chrono::floor<Days>(std::chrono::system_clock::now())};
  }
};

/**
 * @brief Type to represent a period of time with days "resolution".
 * A "Period" is represented as a start "Date"" and a number of "Days"
 */
struct Period {

  /**
   * @brief Convenience type to simplify the task of calculating the "end"
   * date of a period
   */
  using as_days = std::chrono::time_point<std::chrono::system_clock, Days>;

  Period(Date s, Days d) : start_(s), days_(d) { }

  /**
   * @brief The "start" date of a period
   */
  inline Date start() const { return start_; }

  /**
   * @brief The number of "Days"
   */
  inline Days duration() const { return days_; }

  /**
   * @brief The "end" date of a period
   */
  inline Date end() const { return Date{ as_days{start()} + duration() }; }

  /**
   * @brief Check if two periods intersect
   * @return True if the two periods intersect
   */
  bool intersects(const Period& other) const
  {
    return start() < other.end() && other.start() < end();
  }

  /**
   * @brief Check if two periods are the same
   * @return True if the two periods are equal
   */
  bool operator==(const Period& other) const
  {
    return start() == other.start() && duration() == other.duration();
  }

  /**
   * @brief String representation of a Period
   * @return A Period represented as "[StartDate/EndDate]"
   */
  std::string as_string() const
  {
    return std::format("[{}/{}]", start().as_string(), end().as_string());
  }

private:
  Date start_;
  Days days_;
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

using RateDetails = std::map<std::string, Amount>;
struct RateReport {
  Date date;
  Days duration;
  Amount total;
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
