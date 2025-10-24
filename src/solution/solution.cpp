#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	MyPoint target = {0};

  std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  motor1->add_data_callback([&motor1, &target](const uint16_t& data) {
    std::cout << "M1: " << static_cast<int>(data) << "\n";
    if (target.exists) {
    	if (data < 50 ){
		printf("M1: SEND DATA 100");
		motor1->send_data(100);
    	}
	else if (data > 1000){
		printf("M1: SEND DATA 0");
		motor1->send_data(0);

	}
   }
  });
  motor2->add_data_callback([&target](const uint16_t& data) {
    std::cout << "M2: " << static_cast<int>(data) << "\n";
	//printf("Target(%d):(%lf,%lf,%lf)",target.exists,target.x,target.y,target.z);
  });
  commands->add_data_callback([&target](const Point& point) {
    std::cout << "Target:(" << point.x << ", " << point.y << ", " << point.z << ")\n";
    target.x = point.x;
    target.y = point.y;
    target.z = point.z;
    target.exists = 1;
    printf("Motor1 angle: %Lf\n", m1_angle(target) * HUMAN_FRIENDLY_MULTIPLIER);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  return 0;
}

double m1_angle(MyPoint target){
	double x = target.x;
	double y = target.y;
	if (y == 0){
		if (x >= 0)
			return 0;
		else //(x < 0)
			return M_PIl;
	}
	if (x == 0){
		if (y > 0) 
			return M_PIl * (1.5);
		else //(y < 0)
			return M_PIl * (0.5);
	}
	double angle = 0;
	if (x > 0 && y < 0){
		angle += 0;
		angle += atan((-1*y) / x);
		return angle;
	}
	if (x < 0 && y < 0){
		angle += M_PIl * (0.5);
		angle += atan((-1*x) / (-1*y));
		return angle;
	}
	if (x < 0 && y > 0){
		angle += M_PIl;
		angle += atan(y / (-1*x));
		return angle;
	}
	else if (x > 0 && y > 0){
		angle += M_PIl * (1.5);
		angle += atan(x / y);
		return angle;
	}
}
