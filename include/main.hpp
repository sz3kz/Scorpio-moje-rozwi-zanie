#pragma once

// Main header to make testing easier

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "basic_tester.hpp"
#include "solver.hpp"
#include "tester.hpp"

inline void strip(std::string& str) {
  str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
  str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
}

template<typename T>
std::optional<std::vector<std::pair<double, Point>>> parse(T&& input_stream) {
  std::string line;
  std::vector<std::pair<double, Point>> signals;
  while (std::getline(input_stream, line)) {
    strip(line);
    if (line.empty()) {
      continue;
    }
    std::istringstream line_stream(line);
    double delay;
    Point point;
    line_stream >> delay >> point.x >> point.y >> point.z;
    if (line_stream.fail() || !line_stream.eof()) {
      std::cerr << "Invalid input format\n";
      return std::nullopt;
    }
    if (delay < 0.0) {
      std::cerr << "Delay must be non-negative\n";
      return std::nullopt;
    }
    signals.emplace_back(delay, point);
  }
  return signals;
}

template<typename T>
void run_tests(T&& input_stream, double encoder_period, bool debug, const Constraints& constraints, bool preempt) {
  static_assert(std::is_base_of_v<std::istream, std::decay_t<T>>,
    "Input type must be derived from std::istream");
  auto signals = parse(std::forward<T>(input_stream));
  if (!signals.has_value()) {
    return;
  }
  auto tester = std::make_shared<backend::BasicTester>(encoder_period, signals.value(), debug, constraints);
  solver(tester, preempt);
}
