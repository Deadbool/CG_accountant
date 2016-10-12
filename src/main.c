#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Game.h"

int main()
{
	Game game;
	Game_init(&game);

	Move move;

	while (1) {

		Game_read_intpus(&game);
		Game_set_from_inputs(&game);

		//monte_carlo(&game, &move);

		/*move.shoot = TRUE;
		move.val = game.ecount-1;
		move.angle = 0.0f;
		printf("SHOOT %d\n", move.val);*/

		move.shoot = FALSE;
		move.val = 0.0f;
		move.angle = 0.0f;
		printf("MOVE %.0f %.0f\n", game.wolff.x, game.wolff.y);

		#if LOCAL_INPUTS
			if(!Simulation_play_turn(&game, &move) || game.dcount <= 0 || game.ecount <= 0) {
				LOG_"\n*** The End ***\n");
				exit(0);
			}
		#endif

		game.turn++;
	}

    return EXIT_SUCCESS;
}
