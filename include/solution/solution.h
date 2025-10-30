#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>
#include <stdbool.h>

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

int angle2rotation(double rad);
bool is_vertical_reached( int target_rotation, int current_rotation);
bool is_horizontal_reached(int target_rotation, int current_rotation);
// WARNING: different angle implementations, different handing requred!
double calculate_angle_horizontal(double x, double y);
double m2_angle(double x, double y, double z);
TargetAngles * create_angles(double x, double y, double z);
Movements * create_movements(void);
void update_movement_horizontal( int * movement, int target_rotation, int current_rotation);
void update_movement_vertical( int * movement, int target_rotation , int current_rotation);
int calculate_true_vertical_rotation(int encoder_vertical_rotation );

#endif
