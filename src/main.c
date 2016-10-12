#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Game.h"

int main()
{
	Game game;
	Game_init(&game);

	while (1) {

		Game_read_intpus(&game);
		Game_set_from_inputs(&game);

		monte_carlo(&game);

		//printf("SHOOT %d\n", game.ecount-1);
		//printf("MOVE 0 0\n");
	}

    return EXIT_SUCCESS;
}
