#pragma once

#include <string>
#include <map>
#include <vector>

#include "Hotel.hh"

/**
 * @brief Implementation of Rate logic.
 *
 * A user of this logic needs to create a rate table like the following:
 *
 * Rate::Table Tab{
 *     {Rate::Type::Base,     ""       , 58.99}, // default nightly rate
 *     {Rate::Type::Base,     "101"    ,100.99}, // premium room rate
 *     {Rate::Type::Capacity, ""       , 0.20},  // percent surcharge per extra bed
 *     {Rate::Type::Amenity,  "Wifi"   ,  5.00}, // per night
 *     {Rate::Type::Amenity,  "Balcony", 15.00}, // per night
 * };
 *
 * Each entry of this table contains a rate description, and entry consists
 * of three columns. The first column is the rate type, followed by a "key"
 * and then followed by a "value".
 * The meaning of the "key" and the "value" changes depending on the  Rate::Type;
 * there are 3 different types:
 *
 *   1. Base:
 *      it's the base rate for a single room per night, the key is a room
 *      identifier and the value is the base rate for that room in currencty. If
 *      the key is an empty string, then this is the default rate.
 *
 *   2. Capacity
 *      This is the additional surcharge per additional unit of capacity of the
 *      room. The "key"" is a room identifier and the "value" is a decimal number
 *      between 0.00 and 1.00, which represents the percentage of the base rate
 *      to be added per unit of capacity, i.e. If a room has a capacity greater than
 *      1, then an additional charge of "base_rate * capacity_rate * (cap-1)" per
 *      night will be added to the total.
 *
 *   3. Amenity
 *      The "key" is the name of the amenity, i.e. "Wifi", "Balcony", etc. And the
 *      value is the price of such amenity per night expressed in currency.
 *
 * Once the "Rate::Calculator" object is constructed, now it should be possible to
 * ask it to calculate the rate for a given room on a given period i.e.
 *
 *     calc.rate_for(room, {2024, 12, 23}, Days{1});
 */

namespace Rate {
  enum class Type {
    Base,
    Capacity,
    Amenity
  };

  struct Entry {
    Type type;
    std::string key;     // room_id, amenity name, or empty for global
    double value;        // price or multiplier
  };

  using Table = std::vector<Entry>;

  struct Calculator {
    Calculator(void*);

    double rate_for(Room& room, const Date& _, const Duration& duration)
      const {
      auto base_rate = base_rate_for(room, _, duration);
      auto capacity_rate = capacity_rate_for(room, _, duration);

      double total = base_rate;

      if (room.capacity > 1)
        total += base_rate * capacity_rate * (room.capacity - 1);

      for (const auto& amenity : room.amenities())
        total += amenity_rate_for(amenity);

      return total * duration.days();
    }

    inline double
    base_rate_for(Room& r, const Date& _, const Duration& dur=Days{1})
      const {
      return base_rates_.count(r.id)
        ? base_rates_.at(r.id) : default_base_rate_;
    }

    inline double
    capacity_rate_for(Room& r, const Date& _, const Duration& dur=Days{1})
      const {
      return capacity_rates_.count(r.id)
        ? capacity_rates_.at(r.id) : default_capacity_rate_;
    }

    inline double
    amenity_rate_for(const Amenity& name)
      const {
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
