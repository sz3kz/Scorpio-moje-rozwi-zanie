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

  motor1->add_data_callback([&motor1, &angles, movements,&horizontal_match](const uint16_t & data) {
    int current_horizontal_rotation = data;
    if (angles == NULL)
	return;
    if (horizontal_match)
    	return;

    update_movement_horizontal(&(movements->horizontal), angles->horizontal, current_horizontal_rotation);
    motor1->send_data(movements->horizontal);
    horizontal_match = is_horizontal_reached(angles->horizontal, data);
    printf("M1: %4d -> %4d\n",current_horizontal_rotation,angles->horizontal);
  });
  motor2->add_data_callback([&motor2,&angles,movements, &vertical_match](const uint16_t& data) {
    int encoded_current_vertical_rotation = data;
    if (angles == NULL)
    	return;
    if (vertical_match)
	return;
    update_movement_vertical(&(movements->vertical), angles->vertical, encoded_current_vertical_rotation);
    motor2->send_data(movements->vertical);
    vertical_match = is_vertical_reached(angles->vertical, encoded_current_vertical_rotation);
    printf("M2: %4d -> %4d\n",encoded_current_vertical_rotation,angles->vertical);
  });
  commands->add_data_callback([&angles, &horizontal_match, &vertical_match](const Point& point) {
    angles = create_angles(point.x, point.y, point.z);
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

bool is_vertical_reached( int target_rotation, int current_rotation){
	current_rotation = calculate_true_vertical_rotation(current_rotation);
	return abs(target_rotation - current_rotation)
		< ANGLE_ACCEPTABLE_DEVIATION;
}

bool is_horizontal_reached(int target_rotation, int current_rotation){
	return abs(target_rotation - current_rotation)
		< ANGLE_ACCEPTABLE_DEVIATION;
}


int calculate_true_vertical_rotation(int encoder_vertical_rotation ){
	if (encoder_vertical_rotation > FULL_ROTATION * (0.75)
			&& encoder_vertical_rotation < FULL_ROTATION)
		return encoder_vertical_rotation - FULL_ROTATION;
	else
		return encoder_vertical_rotation ;
}

void update_movement_vertical( int * movement, int target_rotation , int current_rotation){
	current_rotation = calculate_true_vertical_rotation(current_rotation);
	if (target_rotation < current_rotation)
		*movement = (-1) * abs(*movement);
	else
		*movement = abs(*movement);
}

void update_movement_horizontal( int * movement, int target_rotation, int current_rotation){
	if ( abs(target_rotation - current_rotation) < abs(FULL_ROTATION - target_rotation - current_rotation)){
		if (target_rotation - current_rotation < 0)
			*movement = (-1) * abs(*movement);
		else
			*movement = abs(*movement);
	}
	else{
		if (FULL_ROTATION - target_rotation - current_rotation < 0)
			*movement = abs(*movement);
		else
			*movement = (-1) * abs(*movement);
	}
}

Movements * create_movements(void){
	Movements * ptr = (Movements *) malloc(sizeof(Movements));
	ptr->horizontal = 0;
	ptr->vertical = 0;
	return ptr;
}

TargetAngles * create_angles(double x, double y, double z){
	TargetAngles * ptr = (TargetAngles *) malloc(sizeof(TargetAngles));
	ptr->horizontal = rad2rotation(m1_angle(x, y));
	ptr->vertical = rad2rotation(m2_angle(x,y,z));
	return ptr;
}

double m2_angle(double x, double y, double z){
	if (z == 0)
		return 0;
	if (x == 0 && y == 0){
		if (z > 0)
			return  (0.5) * M_PIl;
		if (z < 0)
			return (-0.5) * M_PIl;
	}
	return atan( z / sqrt(pow(x,2) + pow(y,2)));
}

double m1_angle(double x, double y){
	double angle = 0;
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
