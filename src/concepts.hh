#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <spdlog/spdlog.h>

#include "config.h"

using Date = boost::gregorian::date;
using DateTime = std::chrono::system_clock::time_point;
using Days = boost::gregorian::days;
using Duration = boost::gregorian::date_duration;
using Period = boost::gregorian::date_period;
using string_view = std::string_view;

static Date Today{ boost::gregorian::day_clock::local_day() };

// period string
static std::string (&_pstr)(const Period&)
  = boost::gregorian::to_simple_string;

// date string
static std::string (&_dstr)(const Date&)
  = boost::gregorian::to_iso_extended_string;

static Date (&from_string)(const std::string&)
  = boost::gregorian::from_string;

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
