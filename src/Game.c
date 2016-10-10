#include "Game.h"

void Game_init(Game *game) {

}

void Game_read_intpus(Game *game) {
	Input *in = &game->input;

	// Everything is supposed to be dead
	in->dcount = 0;
	for (int i=0; i < MAX_DATA; i++)
		in->data[i].dead = TRUE;
	in->ecount = 0;
	for (int i=0; i < MAX_ENNEMIES; i++)
		in->enemies[i].life = 0;

	// Getting Wolff's position
	scanf("%d%d", &in->wolff.x, &in->wolff.y);

	// Read datas
	int dataCount;
	int dataId;
	int dataX;
	int dataY;
	scanf("%d", &dataCount);
	for (int i = 0; i < dataCount; i++) {
		scanf("%d%d%d", &dataId, &dataX, &dataY);
		in->data[dataId].dead = FALSE;
		in->data[dataId].point.x = dataX;
		in->data[dataId].point.y = dataY;
		in->dcount = MAX(in->dcount, dataId+1);
	}

	// Read enemies
	int enemyCount;
	int enemyId;
	int enemyX;
	int enemyY;
	int enemyLife;
	scanf("%d", &enemyCount);
	for (int i = 0; i < enemyCount; i++) {
		scanf("%d%d%d%d", &enemyId, &enemyX, &enemyY, &enemyLife);
		in->enemies[enemyId].life = enemyLife;
		in->enemies[enemyId].point.x = enemyX;
		in->enemies[enemyId].point.y = enemyY;
		in->ecount = MAX(in->ecount, enemyId+1);
	}
}

inline void Game_set_from_inputs(Game *game) {
	game->dcount = game->input.dcount;
	game->ecount = game->input.ecount;
	game->wolff = game->input.wolff;
	memcpy(game->data, game->input.data, sizeof(Data)*game->dcount);
	memcpy(game->enemies, game->input.enemies, sizeof(Ennemy)*game->ecount);
}
