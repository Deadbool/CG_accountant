#include "Point.h"

inline float Point_distance2(Point *a, Point *b) {
	return (a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y);
}

inline float Point_distance(Point *a, Point *b) {
	return sqrt(Point_distance2(a, b));
}

inline float Point_angle_to(Point *point, Point *target) {
	return (180 * atan2(target->y - point->y, target->x - point->x) / PI);
}
