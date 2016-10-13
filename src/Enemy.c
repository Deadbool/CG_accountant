#include "Enemy.h"

inline bool Enemy_will_kill_wolff(Point *e, Point *w, float evx, float evy, float wvx, float wvy) {
	float x2 = e->x - w->x;
	float y2 = e->y - w->y;
	float vx2 = evx - wvx;
	float vy2 = evy - wvy;

	float a = vx2*vx2 + vy2*vy2;
	float b = 2.0f * (x2*vx2 + y2*vy2);
	float c = x2*x2 + y2*y2 - ENNEMIES_RANGE_2;
	float delta = b*b - 4.0f*a*c;

	if (delta < 0) {
		return FALSE;
	}

	float sqrtDelta = sqrt(delta);
	float d = 1.0/(2.0*a);
	float t1 = (-b + sqrtDelta)*d;
	float t2 = (-b - sqrtDelta)*d;
	float t = t1 < t2 ? t1 : t2;

	return t >= 0.0f && t <= 1.0f;
}
