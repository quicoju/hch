#pragma once

#include <list>
#include <set>
#include <boost/date_time/gregorian/gregorian.hpp>

using Date = boost::gregorian::date;
using Days = boost::gregorian::days;
using Duration = boost::gregorian::date_duration;
using Reservation = boost::gregorian::date_period;
using Period = boost::gregorian::date_period;

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
using Reservations = std::list<Reservation>;

namespace Amenity {
  static inline std::string AirConditioning = "AirConditioning";
  static inline std::string Balcony = "Balcony";
  static inline std::string Wifi = "Wifi";
  static inline std::string MiniBar = "MiniBar";
};
using Amenities = std::set<std::string>;
