#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <yaml-cpp/yaml.h>

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
  template<typename T>

  /**
   * @brief annotate data into an ostream object as YAML
   *
   * @param the ostream object that will take annotations
   *
   * @param the data that will be annotated
   */
  void to(std::ostream& output, const T& data)
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
}
