#ifndef MY_SOLUTION
#define MY_SOLUTION

#include <cmath>
#include "../../include/backend_interface/tester.hpp"

#define HUMAN_FRIENDLY_MULTIPLIER (180.0/M_PIl)
#define FULL_ROTATION 4096
#define rad2rotation(rad) (static_cast<int>(rad * (4096 / (2 * M_PIl))))

typedef struct TargetAngles {
	int vertical;
	int horizontal;
} TargetAngles;

// WARNING: different angle implementations, different handing requred!
double m1_angle(const Point target);
double m2_angle(const Point target);
TargetAngles * create_angles(const Point point);

#endif
