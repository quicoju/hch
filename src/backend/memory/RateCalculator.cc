#include "RateCalculator.hh"
#include "HotelData.hh"

/**
 * @brief The "src" needs to point to a Rate::Table that looks like the
 * following:
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
 */

namespace Rate {
  Calculator::Calculator(Backend* src)
  {
    for (const auto& rate : src->rates) {
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
}
