#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>

#define HUMAN_FRIENDLY_MULTIPLIER (180.0/M_PIl)
#define FULL_ROTATION 4096
#define rad2rotation(rad) (static_cast<int>(rad * (4096 / (2 * M_PIl))))

typedef struct MyPoint {
	double x;
	double y;
	double z;
	int angle_vertical;
	int angle_horizontal;
	bool exists;
} MyPoint;

// WARNING: different angle implementations, different handing requred!
double m1_angle(MyPoint target);
double m2_angle(MyPoint target);

#endif
