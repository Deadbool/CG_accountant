#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include "Data.h"
#include "Enemy.h"
#include "Params.h"

typedef struct {
	Point wolff;
	int shots;

	int dcount;
	Data data[MAX_DATA];

	int ecount;
	Ennemy enemies[MAX_ENNEMIES];
}Input;

typedef struct {
	int turn;

	Point wolff;
	int shots;

	int dcount;
	Data data[MAX_DATA];

	int ecount;
	Ennemy enemies[MAX_ENNEMIES];

	Input input;
}Game;

void Game_init(Game *game);
void Game_read_intpus(Game *game);
void Game_set_from_inputs(Game *game);

#endif /* SRC_GAME_H_ */
