#include "Hotel.hh"

/**
 * @brief Implementation of Rate logic.
 *
 * A user of this logic needs to create a rate rules table like the
 * following:
 *
 * RateRules rules{
 *     {RateType::Base,     ""       , 58.99}, // default nightly rate
 *     {RateType::Base,     "101"    ,100.99}, // premium room rate
 *     {RateType::Capacity, ""       , 0.20},  // percent surcharge per extra bed
 *     {RateType::Amenity,  "Wifi"   ,  5.00}, // per night
 *     {RateType::Amenity,  "Balcony", 15.00}, // per night
 * };
 *
 * Each entry of this table contains a rule, and each rule contains three
 * values. The first value is the rate type, followed by a "key" and a "value".
 * The meaning of "key" and "value" change depending on the rule type.
 * There are 3 different types of rules:
 *
 *  1. Base:
       it's the base rate for a single room per night expressed in currency, the
       key is a room identifier and the value is the base rate for that room. If
       the key is an empty string, then this is the default rate.

    2. Capacity
       This is the additional surcharge per additional unit of capacity of the
       room. The "key"" is a room identifier and the "value" is a decimal number
       between 0.00 and 1.00, which represents the percentage of additional
       price per unit of capacity, i.e. If a room has a capacity of 4, then
       there will be an additional charge of "base_rate * 3" per night. It's
       3 because the base_rate covers the base case.

    3. Amenity
       The "key" is the name of the amenity, i.e. "Wifi", "Balcony", etc. And the
       value is the price of such amenity per night.
 */

enum class RateType {
  Base,
  Capacity,
  Amenity
};

struct RateRule {
  RateType type;
  std::string key;     // room_id, amenity name, or empty for global
  double value;        // price or multiplier
};

using RateRules = std::vector<RateRule>;

struct RateCalculator {
  RateCalculator(const RateRules& rules)
  {
    // Represent the rules in maps so it's easier to find the
    // overrides, without having to traverse the table many times
    for (const auto& rule : rules) {
      switch (rule.type) {
      case RateType::Base:
        if (rule.key.empty()) default_base_rate_ = rule.value;
        else base_rates_[rule.key] = rule.value;
        break;
      case RateType::Capacity:
        if (rule.key.empty()) default_capacity_rate_ = rule.value;
        else capacity_rates_[rule.key] = rule.value;
        break;
      case RateType::Amenity:
        amenity_rates_[rule.key] = rule.value;
        break;
      }
    }
  }

  double rate_for(Room& room, const Date& _, const Duration& duration)
  const {
    auto base_rate = base_rates_.count(room.id)
      ? base_rates_.at(room.id) : default_base_rate_;

    auto capacity_rate = capacity_rates_.count(room.id)
      ? capacity_rates_.at(room.id) : default_capacity_rate_;

    double total = base_rate;

    if (room.capacity > 1)
      total += base_rate * capacity_rate * (room.capacity - 1);

    for (const auto& amenity : room.amenities()) {
      if (amenity_rates_.count(amenity))
        total += amenity_rates_.at(amenity);
    }

    return total * duration.days();
  }

private:
  std::map<std::string, double> base_rates_;     // room specific base rates
  std::map<std::string, double> capacity_rates_; // room specific capacity rates
  std::map<std::string, double> amenity_rates_;  // amenity specific price
  double default_base_rate_ = 0.0;
  double default_capacity_rate_ = 0.0;
};
