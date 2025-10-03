#pragma once

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
 *  1. Base:
       it's the base rate for a single room per night, the key is a room
       identifier and the value is the base rate for that room in currencty. If
       the key is an empty string, then this is the default rate.

    2. Capacity
       This is the additional surcharge per additional unit of capacity of the
       room. The "key"" is a room identifier and the "value" is a decimal number
       between 0.00 and 1.00, which represents the percentage of the base rate
       to be added per unit of capacity, i.e. If a room has a capacity greater than
       1, then an additional charge of "base_rate * capacity_rate * (cap-1)" per
       night will be added to the total.

    3. Amenity
       The "key" is the name of the amenity, i.e. "Wifi", "Balcony", etc. And the
       value is the price of such amenity per night expressed in currency.
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

    Calculator(const Table& tab)
    {
      // Represent the rates in maps so it's easier to find the
      // overrides, without having to traverse the table many times
      for (const auto& rate : tab) {
        switch (rate.type) {
        case Type::Base:
          if (rate.key.empty()) default_base_rate_ = rate.value;
          else base_rates_[rate.key] = rate.value;
          break;
        case Type::Capacity:
          if (rate.key.empty()) default_capacity_rate_ = rate.value;
          else capacity_rates_[rate.key] = rate.value;
          break;
        case Type::Amenity:
          amenity_rates_[rate.key] = rate.value;
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
}


