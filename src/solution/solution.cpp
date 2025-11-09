#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <stdlib.h>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	Target * target = NULL;
	Movements * movements = create_movements();
	movements->horizontal = MOTOR_MOVE;
	movements->vertical = MOTOR_MOVE;

  	std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  	auto motor1 = tester->get_motor_1();
  	auto motor2 = tester->get_motor_2();
  	auto commands = tester->get_commands();

  	motor1->add_data_callback(
			[&motor1, &target, movements]
			(const uint16_t & data) {
    		int current_horizontal_rotation = data;
		if (target == NULL)
			return;
		if (target->is_horizontal_reached && target->is_vertical_reached){
			puts("TARGET REACHED!");
			return;
		}

    		decide_direction_horizontal(
				    &(movements->horizontal),
				    target->horizontal,
				    current_horizontal_rotation);
		motor1->send_data( movements->horizontal );
		target->is_horizontal_reached = is_horizontal_reached(
				    target->horizontal,
				    current_horizontal_rotation);
		printf("M1: %4d -> %4d\n",
				    current_horizontal_rotation,
				    target->horizontal);
  	});

  	motor2->add_data_callback(
			[&motor2,&target,movements]
			(const uint16_t& data) {
    		int current_vertical_rotation = calculate_true_vertical_rotation(data);
		if (target == NULL || target->is_vertical_reached)
			return;

	    	decide_direction_vertical(
		    &(movements->vertical),
		    target->vertical,
		    current_vertical_rotation);
    		motor2->send_data( movements->vertical );
    		target->is_vertical_reached = is_vertical_reached(
		    target->vertical,
		    current_vertical_rotation);
    		printf("M2: %4d -> %4d\n",
		    current_vertical_rotation,
		    target->vertical);
  	});
  	commands->add_data_callback(
			[&target]
			(const Point& point) {
    		target = create_target(
		    point.x,
		    point.y,
		    point.z);
    		printf("\n");
    		printf(
		    "TARGET:(%lf,%lf,%lf) --> ANGLES:(%4d, %4d)\n",
		    point.x,
		    point.y,
		    point.z,
		    target->horizontal,
		    target->vertical);
  	});
  	std::this_thread::sleep_for(std::chrono::milliseconds(3600 * 1000));
  	free(target);
  	free(movements);
  	return 0;
}

int angle2rotation(double rad){
	return (int) (rad * (FULL_ROTATION / (2*M_PIl) ) );
}

bool is_vertical_reached( int target_rotation, int current_rotation){
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

void decide_direction_vertical( int * movement, int target_rotation , int current_rotation){
	if (target_rotation < current_rotation)
		*movement = (-1) * abs(*movement);
	else
		*movement = abs(*movement);
}

void decide_direction_horizontal( int * movement, int target_rotation, int current_rotation){
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

Target * create_target(double x, double y, double z){
	Target * ptr = (Target *) malloc(sizeof(Target));
	ptr->horizontal = angle2rotation(calculate_angle_horizontal(x, y));
	ptr->vertical = angle2rotation(calculate_angle_vertical(x,y,z));
	ptr->is_horizontal_reached = false;
	ptr->is_vertical_reached = false;
	return ptr;
}

double calculate_angle_vertical(double x, double y, double z){
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

double calculate_angle_horizontal(double x, double y){
	if (y == 0){
		if (x >= 0)
			return QUADRANT_1ST;
		else
			return QUADRANT_3RD;
	}

	if (x == 0){
		if (y > 0) 
			return QUADRANT_4TH;
		else
			return QUADRANT_2ND;
	}

	if (x > 0 && y < 0)
		return QUADRANT_1ST + atan((-1*y) / x);

	if (x < 0 && y < 0)
		return QUADRANT_2ND + atan((-1*x) / (-1*y));

	if (x < 0 && y > 0)
		return QUADRANT_3RD + atan(y / (-1*x));

	if (x > 0 && y > 0)
		return QUADRANT_4TH + atan(x / y);

	// no value should ever fall through every if statement
	puts("SOMETHING WENT TERRIBLY WRONG!");
}
