#ifndef SRC_POINT_H_
#define SRC_POINT_H_

#include "params.h"

typedef struct {
	int x;
	int y;
}Point;

float Point_distance2(Point *a, Point *b);
float Point_distance(Point *a, Point *b);
float Point_angle_to(Point *point, Point *target);


#endif /* SRC_POINT_H_ */
