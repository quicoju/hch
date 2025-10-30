#pragma once

#include <string>
#include <map>
#include <vector>

#include "Room.hh"

/**
 * @brief Implementation of Rate logic.
 *
 * A user of this class needs to provide a "rate table"" each backend
 * provides it's own table implementation, but it's expected to fillout
 * the following information:
 *
 *  - default_base_rate_:
 *      Default base rate (in currency) for the rooms
 *
 *  - base_rates_ :
 *      Room specific base rate, it overrides the default.
 *
 *  - default_capacity_rate:
 *      Ratio (between 0.0 and 1.0) of the base rate to be surcharged
 *      for each additional capacity unit
 *
 *  - capacity_rates_:
 *      Room specific capacity rate, it overrides the default.
 *
 *  - amenity_rates_:
 *      Costs (in currencty) for a "named" amenity, i.e. "Balcony",
 *      "MiniBar", etc.
 *
 * This class will use this information to calculate costs and provide
 * cost details to its users.
 */
namespace Rate {
  struct Calculator {
    Calculator(void*);

    double rate_for(Room& room, const Date& _, const Duration& duration) const
    {
      // TODO: to keep it simple, for now don't pass dates and durations
      // to the rate accessors, but the buisiness might want to include
      // those values in the calculations, but also another approach
      // is that the date and duration are relevant only to the final
      // calculation, which in that case the rate accessors might stay
      // simple as they are.
      auto total = base_rate_for(room) + capacity_rate_for(room);

      for (const auto& amenity : room.amenities())
        total += amenity_rate_for(amenity);

      return total * duration.days(); // maybe consider the date season here
    }

    RateReport
    rate_report_for(Room& room, const Date& _, const Duration& dur) const
    {
      auto days = dur.days();
      RateReport report{
        {"Base", base_rate_for(room) * days},
        {"Capacity", capacity_rate_for(room) * days},
      };
      for (auto& amenity: room.amenities())
        report.emplace(amenity, amenity_rate_for(amenity) * days);

      report.emplace("Total", rate_for(room, _, dur));
      return report;
    }

    inline double
    base_rate_for(Room& r) const
    {
      return base_rates_.count(r.id)
        ? base_rates_.at(r.id) : default_base_rate_;
    }

    inline double
    capacity_rate_for(Room& r) const
    {
      if (r.capacity <= 1) return 0;
      auto cap_rate = capacity_rates_.count(r.id)
        ? capacity_rates_.at(r.id) : default_capacity_rate_;

      return base_rate_for(r) * cap_rate * (r.capacity - 1);
    }

    inline double
    amenity_rate_for(const Amenity& name) const
    {
      return amenity_rates_.count(name)
        ? amenity_rates_.at(name) : 0.00;
    }

  private:
    // Represent the rate table as maps so it's easier to find the
    // overrides, without having to traverse the table many times
    std::map<std::string, double> base_rates_;     // room specific base rates
    std::map<std::string, double> capacity_rates_; // room specific capacity rates
    std::map<std::string, double> amenity_rates_;  // amenity specific price
    double default_base_rate_ = 0.0;
    double default_capacity_rate_ = 0.0;
  };
}
