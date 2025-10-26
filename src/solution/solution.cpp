#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <stdlib.h>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	MyPoint * target = NULL;

  std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  motor1->add_data_callback([&motor1, target](const uint16_t& data) {
    if (target == NULL){
    	// no target yet
	return;
    }
    std::cout << "M1: " << static_cast<int>(data) << "\n";
  });
  motor2->add_data_callback([&target](const uint16_t& data) {
    std::cout << "M2: " << static_cast<int>(data) << "\n";
  });
  commands->add_data_callback([&target](const Point& point) {
    printf("TARGET:(%lf,%lf,%lf)\n", point.x, point.y, point.z);
    target = create_target(point);
    printf("Needed rotations: %d(m1), %d(m2)\n",
			target->angle_horizontal,
			target->angle_vertical);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  free(target);
  return 0;
}
MyPoint * create_target(Point point){
    MyPoint * target = (MyPoint *) malloc(sizeof(MyPoint));
    target->x = point.x;
    target->y = point.y;
    target->z = point.z;
    target->angle_horizontal = 0;
    target->angle_vertical = 0;
    target->exists = 0;
    return target;
}

double m2_angle(MyPoint * target){
	double x = target->x;
	double y = target->y;
	double z = target->z;
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

double m1_angle(MyPoint * target){
	double x = target->x;
	double y = target->y;
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
