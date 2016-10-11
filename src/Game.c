#include "Game.h"

void Game_init(Game *game) {

}

void Game_read_intpus(Game *game) {
	Input *in = &game->input;

	// Getting Wolff's position
	scanf("%d%d", &in->wolff.x, &in->wolff.y);

	in->dcount = 0;
	in->ecount = 0;

	// Read datas
	int dataCount;
	int dataId;
	int dataX;
	int dataY;
	scanf("%d", &dataCount);
	for (int i = 0; i < dataCount; i++) {
		scanf("%d%d%d", &dataId, &dataX, &dataY);
		in->data[in->dcount].point.x = dataX;
		in->data[in->dcount].point.y = dataY;
		in->data[in->dcount].id = dataId;
		in->dcount++;
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
		in->enemies[in->ecount].life = enemyLife;
		in->enemies[in->ecount].point.x = enemyX;
		in->enemies[in->ecount].point.y = enemyY;
		in->enemies[in->ecount].id = enemyId;
		in->ecount++;
	}
}

inline void Game_set_from_inputs(Game *game) {
	game->dcount = game->input.dcount;
	game->ecount = game->input.ecount;
	game->wolff = game->input.wolff;
	memcpy(game->data, game->input.data, sizeof(Data)*game->dcount);
	memcpy(game->enemies, game->input.enemies, sizeof(Ennemy)*game->ecount);
}
