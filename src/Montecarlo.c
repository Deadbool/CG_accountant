#include "Montecarlo.h"

void monte_carlo(Game *game) {
	timeval timer, now;
	gettimeofday(&timer, NULL);
	SET_TIMER(timer, TIMEOUT)
	int tested = 0;

	Solution best_sol, sol;
	float best_score, score;

	best_score = Solution_score(&best_sol, game);

	// We simulate here
	while (1) {

		// New random sol to test
		score = Solution_score(&sol, game);

		// Keep challenger if it's better
		if (score > best_score) {
			best_sol = sol;
			best_score = score;
		}

		tested++;

		// No more time ?
		gettimeofday(&now, NULL);
		if TIME_TO_STOP(timer, now) {
			LOG_"Tested generations: %d\n", tested);
			LOG_"Score: %.2f\n", best_score);
			break;
		}
	}
}

inline bool Montecarlo_play_turn(Game *game, Move *move) {
	int to_remove_size = 0;
	Data to_remove[MAX_DATA];

	/* 1- Ennemies move towards their targets */
	int closest;
	float min_dist, dist;
	for (int e=0; e < game->ecount; e++) {

		// Searching for the closest data
		closest = 0;
		min_dist = Point_distance2(&game->enemies[e].point, &game->data[0].point);
		for (int d=1; d < game->dcount; d++) {
			dist = Point_distance2(&game->enemies[e].point, &game->data[d].point);
			if (dist < min_dist) {
				min_dist = dist;
				closest = d;
			}
		}

		// Move the enemy
		if (Point_move_to(&game->enemies[e].point, &game->data[closest].point, ENNEMIES_STEP)) {
			to_remove[to_remove_size++] = game->data[closest];
		}
	}

	/* 2- Determine my move */ // TODO Heuristic decision
	if (RAND_DOUBLE() < 0.5) {
		move->shoot = TRUE;
		move->val = RAND_INT(game->ecount);
	} else {
		move->shoot = FALSE;
		move->val = RAND_DIST();
		move->angle = RAND_ANGLE();

		Point_move(&game->wolff, move->angle, move->val);
	}

	/* 3- Am I dead ? */
	for (int e=0; e < game->ecount; e++) {
		if (Point_distance2(&game->wolff, &game->enemies[e].point) < ENNEMIES_RANGE_2)
			return FALSE;
	}

	/* 4- Should I shoot or not ?*/
	if (move->shoot) {
		int eid = (int) move->val;
		game->enemies[eid].life -= DAMAGES(Point_distance(&game->wolff, &game->enemies[eid].point));

		/* 5- Kill my target if his life < 0 */
		if (game->enemies[eid].life <= 0) {
			memcpy(&game->enemies[eid], &game->enemies[eid+1], sizeof(Ennemy) * (game->ecount-1-eid));
			game->ecount--;
		}
	}

	/* 6- Enemies collect data points they share coordinates with */
	// TODO remove data

	return TRUE;
}
