#include "motor.hpp"
#include <cmath>

backend::Motor::Motor(int8_t control_signal, uint16_t pose, std::optional<uint16_t> positive_limit,
  std::optional<uint16_t> negative_limit)
: _cs(control_signal), _pose(pose),
  _random_generator(std::random_device{}()),
  _normal_distribution(0.0, 0.6),
  _positive_limit(positive_limit),
  _negative_limit(negative_limit) { }

void backend::Motor::spin(double seconds) {
  constexpr double speed_factor = 0.006;
  const auto cs = _cs.load(std::memory_order_relaxed);
  auto current_pose = _pose.load(std::memory_order_relaxed);
  auto delta = static_cast<int>(cs * cs * seconds * speed_factor);
  if (cs < 0) {
    delta = -delta;
  }
  auto new_pose = current_pose + delta;
  if (_positive_limit.has_value() && delta > 0 && current_pose <= _positive_limit && new_pose > _positive_limit) {
    new_pose = _positive_limit.value();
  }
  if (_negative_limit.has_value() && delta < 0 && current_pose >= _negative_limit && new_pose < _negative_limit) {
    new_pose = _negative_limit.value();
  }
  while (new_pose < 0) {
    new_pose += 4096;
  }
  _pose.store(static_cast<uint16_t>(new_pose % 4096), std::memory_order_relaxed);
  auto pose = static_cast<uint32_t>(_pose.load(std::memory_order_relaxed));
  auto noise = static_cast<uint32_t>(std::round(_normal_distribution(_random_generator)));
  auto noisy_pose = pose + noise;
  while (noisy_pose < 0) { noisy_pose += 4096; }
  if (_data_callback) {
    _data_callback(static_cast<uint16_t>(noisy_pose % 4096));
  }
}
