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

// WARNING: different angle implementations, different handing requred!
double m1_angle(MyPoint target);
double m2_angle(MyPoint target);

#endif
