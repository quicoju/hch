#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace annotate {
  template<typename T>
  void to(std::ostream& output, const T& data)
  {
    YAML::Node node;
    node = data;
    output << YAML::Dump(node);
  }

  template<typename T>
  std::string as_string(const T& data)
  {
    std::stringstream ss;
    to(ss, data);
    return ss.str();
  }
}
