#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>
#include "motor.hpp"

namespace backend {
TEST(MotorTest, LimitTest) {
  backend::Motor motor(0, 2048, 4000, 10);
  EXPECT_LE(motor._pose.load(std::memory_order_relaxed) - 2048, 4);
  motor.send_data(100);
  for (int i = 0; i < 1000; ++i) {
    motor.spin(0.1);
  }
  EXPECT_LT(motor._pose.load(std::memory_order_relaxed) - 4000, 4);
  for (int i = 0; i < 1000; ++i) {
    motor.spin(0.1);
  }
  motor.send_data(-100);
  for (int i = 0; i < 1000; ++i) {
    motor.spin(0.1);
  }
  EXPECT_LT(motor._pose.load(std::memory_order_relaxed) - 10, 4);
}

TEST(MotorTest, LimitTest2) {
  backend::Motor motor(10, 10, 30, 4000);
  EXPECT_LE(motor._pose.load(std::memory_order_relaxed) - 10, 4);
  motor.send_data(100);
  for (int i = 0; i < 1000; ++i) {
    motor.spin(0.1);
  }
  EXPECT_LT(motor._pose.load(std::memory_order_relaxed) - 30, 4);
  for (int i = 0; i < 1000; ++i) {
    motor.spin(0.1);
  }
  motor.send_data(-100);
  for (int i = 0; i < 1000; ++i) {
    motor.spin(0.1);
  }
  EXPECT_LT(motor._pose.load(std::memory_order_relaxed) - 4000, 4);
}
}  // namespace backend
