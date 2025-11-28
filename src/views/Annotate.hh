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

  template<>
  inline void to<RateReport>(std::ostream& output, const RateReport& r)
  {
    using namespace YAML;
    Emitter e;
    output << (e
      << Precision(10)
      << BeginMap
      << Key << "Rates" << Value
         << BeginMap
         << Key << "date"    << Value << _dstr(r.date)
         << Key << "days"    << Value << r.duration.days()
         << Key << "total"   << Value << r.total
         << Key << "details" << Value << r.details
         << EndMap
      << EndMap).c_str();
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
