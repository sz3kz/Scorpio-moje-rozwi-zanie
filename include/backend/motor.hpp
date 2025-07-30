#pragma once

#include <atomic>
#include <cstdint>
#include <optional>
#include <random>
#include <utility>
#include "component.hpp"
#include "spinnable.hpp"

namespace backend {
class Motor final : public backend_interface::Component<int8_t, uint16_t>, public Spinnable {
  #ifdef FRIEND_TEST
    FRIEND_TEST(MotorTest, LimitTest);
    FRIEND_TEST(MotorTest, LimitTest2);
  #endif
  friend class BasicTester;
  std::atomic<int8_t> _cs;
  std::atomic<uint16_t> _pose;
  std::mt19937 _random_generator;
  std::normal_distribution<double> _normal_distribution;
  std::function<void(const uint16_t&)> _data_callback;
  std::optional<uint16_t> _positive_limit;
  std::optional<uint16_t> _negative_limit;

public:
  Motor(int8_t control_signal, uint16_t pose, std::optional<uint16_t> positive_limit = std::nullopt,
    std::optional<uint16_t> negative_limit = std::nullopt);

  void spin(double) override;
  inline void add_data_callback(std::function<void(const uint16_t&)> data_callback) override  {
    _data_callback = std::move(data_callback);
  }
  inline void send_data(const int8_t& control_signal) override {
    _cs.store(control_signal, std::memory_order_relaxed);
  }
};
}  // namespace backend
