#ifndef SRC_MOVE_H_
#define SRC_MOVE_H_

#include "Game.h"
#include "Params.h"

#define MAX_STEP_FAVORIZATION 6.0f

#define RAND_ANGLE() BRAND_DOUBLE(0.0, 360.0)
#define RAND_DIST() BRAND_DOUBLE(0.0, (WOLFF_STEP * MAX_STEP_FAVORIZATION)+1.0f)

typedef struct {
	bool shoot;
	float val;   // (shoot) ? target_id : distance
	float angle; // only used if !shoot
}Move;

void Move_randomize(Move *move, int ecount);

#endif /* SRC_MOVE_H_ */
