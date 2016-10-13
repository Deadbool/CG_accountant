#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Game.h"

int main()
{
	Game game;
	Game_init(&game);

	Solution sol;

	while (1) {

		Game_read_intpus(&game);
		Game_set_from_inputs(&game);

		monte_carlo(&game, &sol);

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

		game.turn++;
	}

    return EXIT_SUCCESS;
}
