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
  });
  motor2->add_data_callback([&target](const uint16_t& data) {
    std::cout << "M2: " << static_cast<int>(data) << "\n";
  });
  commands->add_data_callback([&target](const Point& point) {
    printf("TARGET:(%lf,%lf,%lf)\n", point.x, point.y, point.z);
    target.x = point.x;
    target.y = point.y;
    target.z = point.z;
    target.exists = 1;
    printf("Needed rotations: %Lf(m1), %Lf(m2)\n",
			m1_angle(target) * HUMAN_FRIENDLY_MULTIPLIER,
			m2_angle(target) * HUMAN_FRIENDLY_MULTIPLIER);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  return 0;
}

double m2_angle(MyPoint target){
	double x = target.x;
	double y = target.y;
	double z = target.z;
	if (z == 0){
		puts("z is zero!");
		return 0;
	}
	if (x == 0 && y == 0){
		puts("target above / below!");
		if (z > 0)
			return M_PIl * (0.5);
		if (z < 0)
			return (-1) * M_PIl * (0.5);
	}
	if (z > 0){
		puts("Cond 1");
		return atan( z / sqrt(pow(x,2) + pow(y,2)));
	}
	puts("Cond 2");
	return (-1) * atan( z / sqrt(pow(x,2) + pow(y,2)));
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
