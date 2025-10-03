#include "RateCalculator.hh"

namespace Rate {
  Calculator::Calculator(void* src)
  {
    auto& table = *static_cast<Rate::Table*>(src);

    for (const auto& rate : table) {
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
