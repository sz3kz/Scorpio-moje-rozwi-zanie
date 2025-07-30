#include <chrono>
#include <exception>
#include <iostream>
#include <utility>
#include "basic_tester.hpp"
#include "solver.hpp"

backend::BasicTester::BasicTester(double encoder_period, std::vector<std::pair<double, Point>> signals,
  bool debug, Constraints constraints)
  : _motor1(std::make_shared<backend::Motor>(0, 0, constraints.vertical_max, constraints.vertical_min)),
    _motor2(std::make_shared<backend::Motor>(0, 0, constraints.horizontal_max, constraints.horizontal_min)),
    _commands(std::make_shared<backend::MockComponent<backend_interface::Tester::Impossible, Point>>()),
    _stop(false),
    _spinner(&BasicTester::spinner, this, encoder_period, debug),
    _sender(&BasicTester::sender, this, std::move(signals)) { }

backend::BasicTester::~BasicTester() {
  _stop.store(true, std::memory_order_relaxed);
  _stop_cv.notify_all();
  _sender.join();
  _spinner.join();
}

void backend::BasicTester::spinner(const double encoder_period, const bool debug) {
  const auto frame_duration = std::chrono::duration<double>(encoder_period);
  double time_accumulator = 0.0;
  while (!_stop.load(std::memory_order_relaxed)) {
    const auto start = std::chrono::steady_clock::now();
    _motor1->spin(static_cast<double>(frame_duration.count()));
    _motor2->spin(static_cast<double>(frame_duration.count()));
    if (debug) {
      std::cout << '[' << encoder_period << "] " << std::to_string(_motor1->_cs.load(std::memory_order_relaxed)) << " "
      << _motor1->_pose.load(std::memory_order_relaxed) << " "
      << std::to_string(_motor2->_cs.load(std::memory_order_relaxed)) << " "
      << _motor2->_pose.load(std::memory_order_relaxed) << std::endl;
    }
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = end - start;
    if (elapsed > frame_duration) {
      std::cerr << "Terminating spinner - callbacks lasted for too long\n" << std::flush;
      std::terminate();
    }
    std::unique_lock<std::mutex> lock(_stop_mutex);
    _stop_cv.wait_for(lock, frame_duration - elapsed, [this]() { return _stop.load(std::memory_order_relaxed); });
    time_accumulator += encoder_period;
  }
}

void backend::BasicTester::sender(std::vector<std::pair<double, Point>> signals) {
  for (size_t i = 0; !_stop.load(std::memory_order_relaxed) && i < signals.size(); ++i) {
    const auto [delay, point] = signals[i];
    std::unique_lock<std::mutex> lock(_stop_mutex);
    _stop_cv.wait_for(lock, std::chrono::duration<double>(delay),
      [this]() { return _stop.load(std::memory_order_relaxed); });
    if (_stop.load(std::memory_order_relaxed)) {
      break;
    }
    _commands->send_to_callback(point);
  }
}
