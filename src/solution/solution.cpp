#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <stdlib.h>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	TargetAngles * angles = NULL;
	Movements * movements = create_movements();
	movements->horizontal = MOTOR_MOVE;
	movements->vertical = MOTOR_MOVE;
	bool horizontal_match;
	bool vertical_match;

  std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  auto motor1 = tester->get_motor_1();
  auto motor2 = tester->get_motor_2();
  auto commands = tester->get_commands();

  motor1->add_data_callback([&motor1, &angles, movements,&horizontal_match](const uint16_t& data) {
    if (angles == NULL)
	return;
    if (horizontal_match)
    	return;

    update_movement_horizontal(movements, angles, data);
    motor1->send_data(movements->horizontal);
    horizontal_match = check_horizontal_match(angles, data);
    printf("M1: %4d -> %4d\n",data,angles->horizontal);
  });
  motor2->add_data_callback([&motor2,&angles,movements, &vertical_match](const uint16_t& data) {
    if (angles == NULL)
    	return;
    if (vertical_match)
	return;
    update_movement_vertical(movements, angles, data);
    motor2->send_data(movements->vertical);
    vertical_match = check_vertical_match(angles, data);
    printf("M2: %4d -> %4d\n",data,angles->vertical);
  });
  commands->add_data_callback([&angles, &horizontal_match, &vertical_match](const Point& point) {
    angles = create_angles(point);
    printf(	"\nTARGET:(%lf,%lf,%lf)"
		" --> "
		"ANGLES:(%4d, %4d)\n", point.x, point.y, point.z,
		angles->horizontal, angles->vertical);
    horizontal_match = false;
    vertical_match = false;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  free(angles);
  free(movements);
  return 0;
}

int rad2rotation(double rad){
	return (int) (rad * (4096 / (2*M_PIl) ) );
}

bool check_vertical_match( TargetAngles * angles, int m2){
	int signed_m2 = get_signed_m2(m2);
	return abs(angles->vertical - signed_m2) < ANGLE_ACCEPTABLE_DEVIATION;
}

bool check_horizontal_match(TargetAngles * angles, int m1){
	return abs(angles->horizontal - m1) < ANGLE_ACCEPTABLE_DEVIATION;
}


int get_signed_m2(int m2){
	int signed_m2;
	if (m2 > FULL_ROTATION * (0.75) && m2 < FULL_ROTATION)
		signed_m2 = m2 - FULL_ROTATION;
	else
		signed_m2 = m2;
	return signed_m2;
}

void update_movement_vertical( Movements * movements, TargetAngles * angles, int m2){
	if (movements == NULL || angles == NULL)
		return;

	int signed_m2 = get_signed_m2(m2);

	if (angles->vertical < signed_m2 )
		movements->vertical = (-1) * abs(movements->vertical);
	else
		movements->vertical = abs(movements->vertical);
}

void update_movement_horizontal( Movements * movements, TargetAngles * angles, int m1){
	if (movements == NULL && angles == NULL)
		return;
	if ( abs(angles->horizontal - m1) < abs(FULL_ROTATION - angles->horizontal - m1)){
		if (angles->horizontal - m1 < 0)
			movements->horizontal = -1 * abs(movements->horizontal);
		else
			movements->horizontal = abs(movements->horizontal);
	}
	else{
		if (FULL_ROTATION - angles->horizontal - m1 < 0)
			movements->horizontal = abs(movements->horizontal);
		else
			movements->horizontal = -1 *  abs(movements->horizontal);
	}
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
	return atan( z / sqrt(pow(x,2) + pow(y,2)));
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
