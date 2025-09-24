#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>

using Date = boost::gregorian::date;
using Days = boost::gregorian::days;
using Duration = boost::gregorian::date_duration;
using Reservation = boost::gregorian::date_period;
using Period = boost::gregorian::date_period;

static std::string (&str)(const Period&)
  = boost::gregorian::to_simple_string;

static std::string (&str2)(const Date&)
  = boost::gregorian::to_iso_extended_string;

static Date (&from_string)(const std::string&)
  = boost::gregorian::from_string;
