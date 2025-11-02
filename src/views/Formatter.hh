#pragma once
#include "concepts.hh"

// output formatter
// ================
// TODO: possibly refactor a serializer class in the future
//       It may also be possible that external dependecies
//       are used to serialze the containers, instead of
//       implementing our own serializers, but for now I'll
//       make a simple one to shape the interface of the
//       Formatter
class Formatter {
public:
  Formatter(): format_{"yaml"} {}

  /* serialize a Container with a transfomation function F,
   * this function stringifies (serializes) an element
   * in the container
   */
  template<typename Container, typename F>
  void output(const Container& items, F f)
  {
    if (format_ == "yaml") {
      std::cout << "---" << std::endl;
      for (const auto& item: items)
        std::cout << "- " << f(item) << std::endl;
    }
  }

  /* serialize a Container, it provides a default serialization function
   */
  template<typename Container>
  requires std::ranges::range<Container>
    && std::convertible_to<typename Container::value_type, std::string>
  void output(const Container& items)
  {
    output(items, [](const auto& x) { return std::string{x}; });
  }

  void output(std::pair<std::string, double> p) {
    output<std::string, double>(p);
  }

  template<typename U, typename V>
  requires std::convertible_to<U, std::string_view> &&
           (std::is_arithmetic_v<V> || std::convertible_to<V, double>)
  void output(const std::pair<U, V>& p) {
  output(std::map<U, V>{{p.first, p.second}});
}

  template<typename K, typename V>
  requires std::convertible_to<K, std::string_view>
       && (std::is_arithmetic_v<V> || std::convertible_to<V, double>)
  void output(const std::map<K, V>& m, const char* indent="")
  {
    std::cout.setf(std::ios::fixed);
    std::cout.precision(2);

    if (format_ == "yaml") {
      for (const auto& [key, value] : m) {
        std::string_view k = key;
        if constexpr (std::is_arithmetic_v<V>) {
          std::cout << indent << k << ": " << value << std::endl;
        } else {
          std::cout << indent << k << ": " << static_cast<double>(value) << std::endl;
        }
      }
    }
  }

  // TODO: This output formatter is temporary, while I use a third
  // party serializer. The problem with this formatter is that it
  // knows too much details about the application, this particular
  // case, it needs to know how to serialize a "RateReport" type
  void output(const RateReport report)
  {
    if (format_ == "yaml") {
      std::cout
        << "---\n"
        << "date: "    << _dstr(report.date) << "\n"
        << "days: "    << report.duration.days() << "\n"
        << "total: "   << report.total << "\n"
        << "details: " << "\n";
      output(report.details, "    ");
    }
  }

private:
  std::string format_;
};
