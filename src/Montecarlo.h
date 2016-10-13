#ifndef SRC_MONTECARLO_H_
#define SRC_MONTECARLO_H_

#include "Params.h"
#include "Solution.h"
#include "Simulation.h"

void monte_carlo(Game *game, Solution *choosen_sol);

bool Montecarlo_play_turn(Game *game, Move *move, float *score);

float Montecarlo_try(Solution *sol, Game *game);

#endif /* SRC_MONTECARLO_H_ */
