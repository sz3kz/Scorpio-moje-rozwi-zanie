#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>
#include <stdbool.h>
#include "../../include/backend_interface/tester.hpp"

#define HUMAN_FRIENDLY_MULTIPLIER (180.0/M_PIl)
#define FULL_ROTATION 4096
#define ANGLE_ACCEPTABLE_DEVIATION 5
#define MOTOR_MOVE 100


typedef struct TargetAngles {
	int vertical;
	int horizontal;
} TargetAngles;

typedef struct Movements {
	int vertical;
	int horizontal;
} Movements;

int rad2rotation(double rad);
bool check_vertical_match( int target_rotation, int current_rotation);
bool check_horizontal_match(int target_rotation, int current_rotation);
// WARNING: different angle implementations, different handing requred!
double m1_angle(double x, double y);
double m2_angle(double x, double y, double z);
TargetAngles * create_angles(const Point point);
Movements * create_movements(void);
void update_movement_horizontal( int * movement, int target_rotation, int current_rotation);
void update_movement_vertical( int * movement, int target_rotation , int current_rotation);
int get_real_m2(int m2);

#endif
