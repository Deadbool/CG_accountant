#include "Game.h"

void Game_init(Game *game) {
	game->turn = 0;
	game->input.total_life = 0;
	game->input.shots = 0;
	game->input.score = 0.0f;

	#if LOCAL_INPUTS
		FILE *test = fopen(TEST_CASE, "r");
		FILE *simu = fopen("simulation/simu.txt", "w");
		if (test == NULL || simu == NULL) {
			printf("Unable to open file\n");
			exit(0);
		}

		// Copy
		char c;
		do {
			c = fgetc(test);
			fputc(c, simu);
		} while (c != EOF);

		fclose(test);
		fclose(simu);
	#endif
}

void Game_read_intpus(Game *game) {
	Input *in = &game->input;

	FILE *f = NULL;
	#if LOCAL_INPUTS
		f = fopen("simulation/simu.txt", "r");
		if (f == NULL) {
			printf("Unable to open simu.txt file\n");
			exit(0);
		}
	#else
		f = stdin;
	#endif

	// Getting Wolff's position
	fscanf(f, "%d%d", &in->wolff.x, &in->wolff.y);

	#if	LOG_INPUTS
	LOG_"=== Inputs ===\n");
		LOG_"%d %d\n", in->wolff.x, in->wolff.y);
	#endif

	in->dcount = 0;
	in->ecount = 0;

	// Read datas
	int dataCount;
	int dataId;
	int dataX;
	int dataY;
	fscanf(f, "%d", &dataCount);
	#if	LOG_INPUTS
		LOG_"%d\n", dataCount);
	#endif
	for (int i = 0; i < dataCount; i++) {
		fscanf(f, "%d%d%d", &dataId, &dataX, &dataY);

		#if	LOG_INPUTS
			LOG_"%d %d %d\n", dataId, dataX, dataY);
		#endif

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
	fscanf(f, "%d", &enemyCount);
	#if	LOG_INPUTS
		LOG_"%d\n", enemyCount);
	#endif
	for (int i = 0; i < enemyCount; i++) {
		fscanf(f, "%d%d%d%d", &enemyId, &enemyX, &enemyY, &enemyLife);

		#if	LOG_INPUTS
			LOG_"%d %d %d %d\n", enemyId, enemyX, enemyY, enemyLife);
		#endif

		in->enemies[in->ecount].life = enemyLife;
		in->enemies[in->ecount].point.x = enemyX;
		in->enemies[in->ecount].point.y = enemyY;
		in->enemies[in->ecount].id = enemyId;
		in->ecount++;

		in->total_life += (game->turn > 0) ? 0 : enemyLife;
	}

	#if	LOG_INPUTS
		LOG_"==============\n");
	#endif

	#if LOCAL_INPUTS
		fclose(f);
	#endif
}

inline void Game_set_from_inputs(Game *game) {
	game->dcount = game->input.dcount;
	game->ecount = game->input.ecount;
	game->wolff = game->input.wolff;
	game->shots = game->input.shots;
	memcpy(game->data, game->input.data, sizeof(Data)*game->dcount);
	memcpy(game->enemies, game->input.enemies, sizeof(Ennemy)*game->ecount);
}


