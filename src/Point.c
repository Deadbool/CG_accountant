#include "Point.h"

inline float Point_distance2(Point *a, Point *b) {
	return (a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y);
}

inline float Point_distance(Point *a, Point *b) {
	return floorf(sqrt(Point_distance2(a, b)));
}

inline float Point_angle_to(Point *point, Point *target) {
	return (180 * atan2(target->y - point->y, target->x - point->x) / PI);
}

inline void Point_move(Point *point, float angle, float dist) {
	double rad = DEG_TO_RAD(angle);
	point->x = (int) (cos(rad) * dist + point->x);
	point->y = (int) sin(rad) * dist + point->y;

	if (point->x < 0)
		point->x = 0;
	else if (point->x >= MAP_W)
		point->x = MAP_W-1;

	if (point->y < 0)
		point->y = 0;
	else if (point->y >= MAP_H)
		point->y = MAP_H-1;
}

inline bool Point_move_to(Point *point, Point *target, float dist) {
	float d = Point_distance(point, target);
	if (d <= dist) {
		point->x = target->x;
		point->y = target->y;
		return TRUE;
	}

	point->x = (int) (point->x + ((target->x - point->x) * dist / d));
	point->y = (int) (point->y + ((target->y - point->y) * dist / d));

	if (point->x < 0)
		point->x = 0;
	else if (point->x >= MAP_W)
		point->x = MAP_W-1;

	if (point->y < 0)
		point->y = 0;
	else if (point->y >= MAP_H)
		point->y = MAP_H-1;

	return FALSE;
}
