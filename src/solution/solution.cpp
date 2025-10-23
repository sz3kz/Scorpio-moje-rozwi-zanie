#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	MyPoint target;
	target.x = 0;
	target.y = 0;
	target.z = 0;

  std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  motor1->add_data_callback([](const uint16_t& data) {
    std::cout << "Motor 1 data: " << static_cast<int>(data) << "\n";
  });
  motor2->add_data_callback([&target](const uint16_t& data) {
    std::cout << "Motor 2 data: " << static_cast<int>(data) << "\n";
    printf("Point's x is %lf\n",target.x);
  });
  commands->add_data_callback([&target](const Point& point) {
    std::cout << "Command point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
    target.x = point.x;
    target.y = point.y;
    target.z = point.z;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  return 0;
}
