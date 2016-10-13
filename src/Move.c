#include "Move.h"

inline void Move_randomize(Move *move, int ecount) {
	if (RAND_DOUBLE() < 0.5) {
		move->shoot = TRUE;
		move->val = RAND_INT(ecount);
	} else {
		move->shoot = FALSE;
		move->val = RAND_DIST();
		if (move->val > WOLFF_STEP)
			move->val = WOLFF_STEP;
		move->angle = RAND_ANGLE();
	}
}
