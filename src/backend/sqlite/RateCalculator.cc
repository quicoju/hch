#include "RateCalculator.hh"
#include "SQLite.hh"

namespace Rate {

  Calculator::Calculator(void* src) {
    auto& db = *static_cast<SQLite*>(src);

    auto stmt = db.prepare(R"(
SELECT rt.name, r.key_name, r.value
  FROM rates r
  JOIN rate_types rt ON r.type_id = rt.id
 ORDER BY rt.id, r.key_name
)");

    while (stmt.next()) {
      auto type = stmt.get<std::string>(0);
      auto key = stmt.get<std::string>(1);
      auto value = stmt.get<double>(2);

      if (type == "Base") {
        if (key.empty()) default_base_rate_ = value;
        else base_rates_[key] = value;
      }
      else if (type == "Capacity") {
        if (key.empty()) default_capacity_rate_ = value;
        else capacity_rates_[key] = value;
      }
      else if (type == "Amenity") {
        amenity_rates_[key] = value;
      }
      else ; // TODO: unkown type, maybe log it
    }
  }
}
