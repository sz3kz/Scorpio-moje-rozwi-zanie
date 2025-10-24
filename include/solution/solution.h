#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>

#define HUMAN_FRIENDLY_MULTIPLIER (180.0/M_PIl)

typedef struct MyPoint {
	double x;
	double y;
	double z;
	bool exists;
} MyPoint;

double m1_angle(MyPoint target);

#endif
