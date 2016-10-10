#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include "Data.h"
#include "Enemy.h"

#define MAX_DATA 100
#define MAX_ENNEMIES 100

#define WOLFF_STEP 1000.0f
#define ENNEMIES_RANGE 2000.0f
#define ENNEMIES_STEP 500.0f

#define DATA_VALUE 100.0f
#define KILL_VALUE 10.0f
#define FINAL_SCORE(dp, l, s) (dp * MAX(0.0f, (l - 3.0f*s)) * 3.0f)

typedef struct {
	Point wolff;

	int dcount;
	Data data[MAX_DATA];

	int ecount;
	Ennemy enemies[MAX_ENNEMIES];
}Input;

typedef struct {
	Point wolff;

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
