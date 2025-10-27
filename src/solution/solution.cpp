#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <stdlib.h>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	TargetAngles * angles = NULL;
	Movements * movements = create_movements();
	movements->horizontal=100;

  std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  motor1->add_data_callback([&motor1, &angles, movements](const uint16_t& data) {
    if (angles == NULL)
	return;
    printf("M1: %4d -> %4d\n",data,angles->horizontal);
    motor1->send_data(movements->horizontal);
  });
  motor2->add_data_callback([&motor2,&angles,movements](const uint16_t& data) {
    if (angles == NULL)
    	return;
    printf("M2: %4d -> %4d\n",data,angles->vertical);
    motor2->send_data(movements->vertical);
  });
  commands->add_data_callback([&angles](const Point& point) {
    angles = create_angles(point);
    printf(	"\nTARGET:(%lf,%lf,%lf)"
		" --> "
		"ANGLES:(%4d, %4d)\n", point.x, point.y, point.z,
		angles->horizontal, angles->vertical);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  free(angles);
  free(movements);
  return 0;
}

Movements * create_movements(void){
	Movements * ptr = (Movements *) malloc(sizeof(Movements));
	ptr->horizontal = 0;
	ptr->vertical = 0;
	return ptr;
}

TargetAngles * create_angles(const Point point){
	TargetAngles * ptr = (TargetAngles *) malloc(sizeof(TargetAngles));
	ptr->horizontal = rad2rotation(m1_angle(point));
	ptr->vertical = rad2rotation(m2_angle(point));
	return ptr;
}

double m2_angle(const Point target){
	double x = target.x;
	double y = target.y;
	double z = target.z;
	if (z == 0)
		return 0;
	if (x == 0 && y == 0){
		if (z > 0)
			return M_PIl * (0.5);
		if (z < 0)
			return (-1) * M_PIl * (0.5);
	}
	if (z > 0)
		return atan( z / sqrt(pow(x,2) + pow(y,2)));
	return (-1) * atan( z / sqrt(pow(x,2) + pow(y,2)));
}

double m1_angle(const Point target){
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
