#ifndef SRC_POINT_H_
#define SRC_POINT_H_

#include "Params.h"

typedef struct {
	float x;
	float y;
}Point;

float Point_distance2(Point *a, Point *b);
float Point_distance(Point *a, Point *b);
float Point_angle_to(Point *point, Point *target);
void Point_move(Point *point, float angle, float dist);
bool Point_move_to(Point *point, Point *target, float dist);

#endif /* SRC_POINT_H_ */
