#pragma once

#include <list>
#include <set>
#include <boost/date_time/gregorian/gregorian.hpp>

using Date = boost::gregorian::date;
using Days = boost::gregorian::days;
using Duration = boost::gregorian::date_duration;
using Reservation = boost::gregorian::date_period;
using Period = boost::gregorian::date_period;

static Date Today(boost::gregorian::day_clock::local_day());

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

using Amenity = std::string;
using Amenities = std::set<Amenity>;
static inline Amenity AirConditioning = "AirConditioning";
static inline Amenity Balcony = "Balcony";
static inline Amenity Wifi = "Wifi";
static inline Amenity MiniBar = "MiniBar";
