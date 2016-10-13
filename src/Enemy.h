#ifndef SRC_ENEMY_H_
#define SRC_ENEMY_H_

#include "Point.h"

typedef struct {
	Point point;
	int life;
	int id;
}Ennemy;

bool Enemy_will_kill_wolff(Point *e, Point *w, float evx, float evy, float wvx, float wvy);

#endif /* SRC_ENEMY_H_ */
