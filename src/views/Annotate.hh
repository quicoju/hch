#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <yaml-cpp/yaml.h>

#include "Reservation.hh"

/**
 * @brief Add "annotation" capabilities to the project.
 *
 * The idea behind this module is to provide non-member functions that that will
 * allow the project to write data in a readable format (serialization) into an
 * object, which could be a stream, file, etc.
 *
 * To add these capabilities,this module needs to know how to convert different
 * types of objects into strings.
 *
 * Currently annotations will be made in YAML.
 */

namespace annotate {
  /**
   * @brief annotate data into an ostream object as YAML
   *
   * @param the ostream object that will take annotations
   *
   * @param the data that will be annotated
   */
  template<typename T>
  inline void to(std::ostream& output, const T& data)
  {
    YAML::Node node;
    node = data;
    output << YAML::Dump(node);
  }

  /**
   * @brief turn data into a YAML string
   *
   * @param the data that is to be formatted
   *
   * @return the formatted YAML string
   */
  template<typename T>
  std::string as_string(const T& data)
  {
    std::stringstream ss;
    to(ss, data);
    return ss.str();
  }

  inline YAML::Node read(std::string_view s)
  {
    return YAML::Load(s.data());
  }
}

////////////////////////////////////////////////////////////
//  Serialize specific types using the yaml-cpp interface //
////////////////////////////////////////////////////////////

// DateTime to string
static std::string to_string(const DateTime& dt)
{
  return std::format("{:%Y-%m-%d %H:%M:%S}", dt);
}

namespace YAML {
  template<>
  struct convert<std::optional<DateTime>> {
    static Node encode(const std::optional<DateTime>& dt)
    {
      Node node;
      if (dt) node = to_string(dt.value());
      return node;
    }
  };

  template<>
  struct convert<Reservation> {
    static Node encode(const Reservation& r)
    {
      Node node;
      node["id"] = r.id;
      node["guest"] = r.guest_id;
      node["room"] = r.room_id;
      node["checkin_at"] = r.checkin_at;
      node["checkout_at"] = r.checkout_at;
      return node;
    }
  };

  template<>
  struct convert<RateReport> {
    static Node encode(const RateReport& r)
    {
      Node report, details;

      // yaml-cpp doesn't provide a "fixed" manipulator so when
      // a detail has a value without decimals, it'll format it
      // as an integer, so format it manually instead
      for (auto &[k, v]: r.details)
        details[k] = std::format("{:.2f}", v);

      report["date"] = r.date.as_string();
      report["days"] = r.days;
      report["total"] = std::format("{:.2f}", r.total);
      report["details"] = details;

      return report;
    }
  };

  // TODO: see why yml-cpp is not serializing this automatically
  template<>
  struct convert<std::set<std::string>> {
    static Node encode(const std::set<std::string>& s) {
      Node node;
      for (const auto& item : s)
        node.push_back(item);
      return node;
    }
  };

}
