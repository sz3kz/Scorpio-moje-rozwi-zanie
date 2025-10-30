#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>
#include <stdbool.h>

#define HUMAN_FRIENDLY_MULTIPLIER (180.0/M_PIl)
#define FULL_ROTATION 4096
#define ANGLE_ACCEPTABLE_DEVIATION 5
#define MOTOR_MOVE 100


typedef struct Target {
	int vertical;
	int horizontal;
	bool is_vertical_reached;
	bool is_horizontal_reached;
} Target;

typedef struct Movements {
	int vertical;
	int horizontal;
} Movements;

int angle2rotation(double rad);
double calculate_angle_horizontal(double x, double y);
double calculate_angle_vertical(double x, double y, double z);
int calculate_true_vertical_rotation(int encoder_vertical_rotation );

Target * create_target(double x, double y, double z);

Movements * create_movements(void);
void decide_direction_vertical( int * movement, int target_rotation , int current_rotation);
void decide_direction_horizontal( int * movement, int target_rotation, int current_rotation);

bool is_horizontal_reached(int target_rotation, int current_rotation);
bool is_vertical_reached( int target_rotation, int current_rotation);

#endif
