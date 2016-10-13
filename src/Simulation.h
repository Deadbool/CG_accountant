#ifndef SRC_SIMULATION_H_
#define SRC_SIMULATION_H_

#include "Game.h"

void Simulation_play_turn(Game *game, Move *move);

void Simulation_play_turn_with_defined_move(Game *game, int x, int y);

void Simulation_play_turn_with_defined_shot(Game *game, int eid);

void Simulation_output(Game *game);

#endif /* SRC_SIMULATION_H_ */
