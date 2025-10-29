#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>
#include <stdbool.h>
#include "../../include/backend_interface/tester.hpp"

#define HUMAN_FRIENDLY_MULTIPLIER (180.0/M_PIl)
#define FULL_ROTATION 4096
#define ANGLE_ACCEPTABLE_DEVIATION 5
#define MOTOR_MOVE 100
#define rad2rotation(rad) (static_cast<int>(rad * (4096 / (2 * M_PIl))))

typedef struct TargetAngles {
	int vertical;
	int horizontal;
} TargetAngles;

typedef struct Movements {
	int vertical;
	int horizontal;
} Movements;

bool check_vertical_match( TargetAngles * angles, int m2);
bool check_horizontal_match(TargetAngles * angles, int m1);
// WARNING: different angle implementations, different handing requred!
double m1_angle(const Point target);
double m2_angle(const Point target);
TargetAngles * create_angles(const Point point);
Movements * create_movements(void);
void update_movement_horizontal( Movements * movements, TargetAngles * angles, int m1);
void update_movement_vertical( Movements * movements, TargetAngles * angles, int m2);
int get_signed_m2(int m2);

#endif
