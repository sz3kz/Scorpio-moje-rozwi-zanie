#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {

  // Command argument specific header
  std::cout << (preempt ? "Preempt" : "Queue") << '\n';

   // Initialization of camera-controlling components
  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();
  // Initialization of lambda callbacks
  	// A motor callback is meant to represent a function that is ran
	// 	with regular intervals (read frequency), and returns the
	// 	current read of the encoder (rotation of the motor).
  motor1->add_data_callback([](const uint16_t& data) {
    std::cout << "Motor 1 data: " << static_cast<int>(data) << "\n";
  });
  motor2->add_data_callback([](const uint16_t& data) {
    std::cout << "Motor 2 data: " << static_cast<int>(data) << "\n";
  });
	// The command callback is also meant to represent a function, which
	// 	is ran when a point object's data has been successfully inputted
	// 	to the program. This function is also given the data concerning
	// 	the global cordinates of the point in question.
  commands->add_data_callback([](const Point& point) {
    std::cout << "Command point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
  });

  // The send_data functions are meant to send control signals to the
  // 	specific motor, supposedly instructing the motor to move.
  motor1->send_data(100);
  motor2->send_data(-50);

  // From my testing, this line makes the simulation start and continue for
  // 	the amount of given amount of time. This plays directly into the fact
  // 	that points can are to be inputted via the commands callback after
  // 	an arbitrary passage of time. Thus, as not to miss any point, it is
  // 	advantageous for the thread sleep time to be large ( like an hour)
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  return 0;
}
