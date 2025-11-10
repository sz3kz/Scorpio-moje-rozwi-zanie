#include "tester.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <queue>

#include "../../include/solution/solution.h"

int solver(std::shared_ptr<backend_interface::Tester> tester, bool preempt) {
	std::queue<Target *> targets;
	int target_count = 0;
	Movements * movements = create_movements();
	movements->horizontal = MOTOR_MOVE;
	movements->vertical = MOTOR_MOVE;

  	std::cout << (preempt ? "Preempt" : "Queue") << '\n';

  	auto motor1 = tester->get_motor_1();
  	auto motor2 = tester->get_motor_2();
  	auto commands = tester->get_commands();

	/* The primary control logic execution section*/
  	motor1->add_data_callback(
			[&motor1,&motor2, &targets, movements, &target_count]
			(const uint16_t & data) {
    		int current_horizontal_rotation = data;
		Target * target = targets.front();

		/* Check if a target is available */
		if (target == NULL)
			return;

		printf("M1: %4d -> %4d\n",
			current_horizontal_rotation,
			target->horizontal);

		/* Stop camera if in desired horizontal rotation */
		if (target->is_horizontal_reached){
			motor1->send_data(MOTOR_STOP);
			/* Halt motor2 + delete target if in disired position */
			if (target->is_vertical_reached){
				motor2->send_data(MOTOR_STOP);
				destroy_target(target);
				targets.pop();
				target_count--;
				printf("Target reached, %d targets left!\n",target_count);
			}
			return;
		}

		/* Move the camera */
		decide_direction_horizontal(
				    &(movements->horizontal),
				    target->horizontal,
				    current_horizontal_rotation);
		motor1->send_data( movements->horizontal );

		/* Check if camera in desired horizontal rotation */
		target->is_horizontal_reached = is_horizontal_reached(
				    target->horizontal,
				    current_horizontal_rotation);
  	});

  	motor2->add_data_callback(
			[&motor2,&targets,movements]
			(const uint16_t& data) {
    		int current_vertical_rotation = calculate_true_vertical_rotation(data);
		Target * target = targets.front();

		/* Check if a target is available */
		if (target == NULL)
			return;

		printf("M2: %4d -> %4d\n",
			current_vertical_rotation,
			target->vertical);

		/* Stop camera if in desired vertical rotation */
		if (target->is_vertical_reached){
			motor2->send_data(MOTOR_STOP);
			return;
		}

		/* Move the camera */
	    	decide_direction_vertical(
		    &(movements->vertical),
		    target->vertical,
		    current_vertical_rotation);
    		motor2->send_data( movements->vertical );

		/* Check if camera in desired horizontal rotation */
    		target->is_vertical_reached = is_vertical_reached(
		    target->vertical,
		    current_vertical_rotation);
  	});
  	commands->add_data_callback(
			[&targets, preempt, &target_count]
			(const Point& point) {
		Target * target = create_target(
		    point.x,
		    point.y,
		    point.z);

		/* If preempt and previous target exists, delete it*/
		if (preempt && target_count == 1){
			targets.pop();
			target_count--;
		}
		targets.push(target);
		target_count++;
    		printf("\n");
    		printf(
		    "TARGET:(%lf,%lf,%lf) --> ANGLES:(%4d, %4d)\n",
		    point.x,
		    point.y,
		    point.z,
		    targets.front()->horizontal,
		    targets.front()->vertical);
  	});

	std::this_thread::sleep_for(std::chrono::hours(1));
	destroy_movements(movements);
  	return 0;
}

int angle2rotation(double rad){
	/* Converts rad angle value to rotation value.*/
	return (int) (rad * (FULL_ROTATION / (2*M_PIl) ) );
}

bool is_vertical_reached( int target_rotation, int current_rotation){
	/* Checks if camera's and target's vertical rotation are within the 
	 * range of the acceptable rotation deviation.
	 * The vertical rotation values are neat, so no second weird condition
	 * here, unlike with horizontal rotation.
	 * */
	return abs(target_rotation - current_rotation)
		< ROTATION_ACCEPTABLE_DEVIATION;
}

bool is_horizontal_reached(int target_rotation, int current_rotation){
	/* Checks if camera's and target's horizontal rotation are within the 
	 * range of the acceptable rotation deviation.
	 * Since the horizontal rotation wraps around so poorly, we need
	 * to have the second check for an event such as:
	 * target_rotation = 4095
	 * current_rotation = 1*/
	return abs(target_rotation - current_rotation) < ROTATION_ACCEPTABLE_DEVIATION ||
		abs(target_rotation - current_rotation - FULL_ROTATION) < ROTATION_ACCEPTABLE_DEVIATION;
}


int calculate_true_vertical_rotation(int encoder_vertical_rotation ){
	/* Translates data given from motor2's encoder into the format
	 * defined in calculate_angle_vertical*/
	if (encoder_vertical_rotation > FULL_ROTATION * (0.75)
			&& encoder_vertical_rotation < FULL_ROTATION)
		return encoder_vertical_rotation - FULL_ROTATION;
	else
		return encoder_vertical_rotation ;
}

void decide_direction_vertical( int * movement, int target_rotation , int current_rotation){
	/* Checks the relative "rotation-location" of the target and camera in 
	 * order to pick the direction of rotation movement.
	 * Unlike horizontal rotation direction, with the restraints we needed
	 * to give the angle computation, we are certain there is only one way
	 * that leads the camera to the target.
	 * */
	if (target_rotation < current_rotation)
		*movement = (-1) * abs(*movement);
	else
		*movement = abs(*movement);
}

void decide_direction_horizontal( int * movement, int target_rotation, int current_rotation){
	/* Compares rotation distances in order to find the direction, which
	 * will result in the smallest rotation movement needed to reach the 
	 * camera.*/
	if (target_rotation > current_rotation){
		if (target_rotation - current_rotation > FULL_ROTATION * 0.5)
			*movement = (-1) * abs(*movement);
		else
			*movement = abs(*movement);
	}
	else{
		if (current_rotation - target_rotation > FULL_ROTATION * 0.5)
			*movement = abs(*movement);
		else
			*movement = (-1) * abs(*movement);
	}
}

Movements * create_movements(void){
	/* Initializes the Movements structure.*/
	Movements * ptr = (Movements *) malloc(sizeof(Movements));
	ptr->horizontal = 0;
	ptr->vertical = 0;
	return ptr;
}

void destroy_movements(Movements * movements){
	/* Centralizes the destruction process of Movements structures.*/
	free(movements);
}

Target * create_target(double x, double y, double z){
	/* Initializes the Target structure based on available data.*/
	Target * ptr = (Target *) malloc(sizeof(Target));
	ptr->horizontal = angle2rotation(calculate_angle_horizontal(x, y));
	ptr->vertical = angle2rotation(calculate_angle_vertical(x,y,z));
	ptr->is_horizontal_reached = false;
	ptr->is_vertical_reached = false;
	return ptr;
}

void destroy_target(Target * target){
	/* Centralizes the destruction process of Target structures.*/
	free(target);
}

double calculate_angle_vertical(double x, double y, double z){
	/* uses trigonometry to translate 3d-expressed point coordinates
	 * into a vertical angle which motor 2 needs to move the camera.
	 * values are in range: [-pi/2;pi/2]
	 * because motor1 can move the camera 360, it can also flip the
	 * camera, which would be equivalent to the motor2 doing 180.
	 * that is why we need to implement a more restricted range of 
	 * movement for motor2 and let motor1 flip the camera if needed.*/
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
	/* Uses trigonometry to translate 2d-expressed point coordinates
	 * into a horizontal angle, which motor1 needs to move the camera.
	 * Values are in range: [0;2pi) - full range of motion.*/
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
