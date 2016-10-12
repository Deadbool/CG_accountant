#ifndef SRC_MONTECARLO_H_
#define SRC_MONTECARLO_H_

#include "params.h"
#include "Solution.h"

void monte_carlo(Game *game);

bool Montecarlo_play_turn(Game *game, Move *move, float *score);

float Montecarlo_try(Move *first_move, Game *game);

#endif /* SRC_MONTECARLO_H_ */
