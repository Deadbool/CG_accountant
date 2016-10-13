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

	while (1) {

		Game_read_intpus(&game);
		Game_set_from_inputs(&game);

		score = monte_carlo(&game, &sol, score);

		#if LOG_SOLUTION
			for (int i=0; i < sol.size; i++) {
				LOG_"%d) shoot=%d val=%.0f angle=%.2f\n", i,
				sol.moves[i].shoot, sol.moves[i].val, sol.moves[i].angle);
			}
		#endif

		#if LOCAL_INPUTS
			LOG_"\n");

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
		memcpy(sol.moves, &sol.moves[1], sizeof(Move) * (sol.size - 1));
		sol.moves[sol.size - 1].shoot = FALSE;
		sol.moves[sol.size - 1].val = 0.0f;
		sol.moves[sol.size - 1].angle = 0.0f;

		game.turn++;
	}

    return EXIT_SUCCESS;
}
