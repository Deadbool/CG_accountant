#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Game.h"

int main()
{
	Game game;
	Game_init(&game);

	Solution sol;
	float score = -(MAX_SCORE * 2);
	for (int i=0; i < MAX_DEPTH; i++) {
		sol.moves[i].shoot = FALSE;
		sol.moves[i].val = 0.0f;
		sol.moves[i].angle = 0.0f;
	}

	/*game.input.total_life = 115;
	game.input.score = 0;
	game.input.shots = 1;
	game.turn = 1;*/

	while (1) {

		Game_read_intpus(&game);
		Game_set_from_inputs(&game);

		if (game.turn == 0)
			LOG_">>> Total life: %d <<<\n", game.input.total_life);

		LOG_"Seed: %d\n", g_seed);

		//if (game.turn == 0)
		score = monte_carlo(&game, &sol, score);

		if (sol.moves[0].shoot) {
			game.input.shots++;

			int p = (int) sol.moves[0].val;

			if (game.enemies[p].life <= DAMAGES(Point_distance(&game.wolff, &game.enemies[p].point)))
				game.input.score += KILL_VALUE;

			printf("SHOOT %d\n", game.enemies[p].id);
		} else {
			Point_move(&game.wolff, sol.moves[0].angle, sol.moves[0].val);
			printf("MOVE %.0f %.0f\n", game.wolff.x, game.wolff.y);
		}

		Game_set_from_inputs(&game);

		#if LOG_SOLUTION
			for (int i=0; i < sol.size; i++) {
				LOG_"%d) shoot=%d val=%.0f angle=%.2f\n", i,
				sol.moves[i].shoot, sol.moves[i].val, sol.moves[i].angle);
			}
		#endif

		#if LOCAL_INPUTS
			LOG_"\n");

			/*if (sol.size == 3) {
				int bk = 0;
			}*/

			Simulation_play_turn(&game, &sol.moves[0]);

			if(game.dcount <= 0) {
				LOG_"*** No more data ***\n");
				exit(0);
			}

			if(game.ecount <= 0) {
				LOG_"*** No more enemies ***\n");
				exit(0);
			}
		#endif

		// Shift the solution for next turn
		memmove(sol.moves, &sol.moves[1], sizeof(Move) * (sol.size - 1));
		sol.size--;

		game.turn++;
	}

    return EXIT_SUCCESS;
}
