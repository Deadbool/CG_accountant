#include "Montecarlo.h"

void monte_carlo(Game *game, Move *choosen_move) {
	timeval timer, now;
	gettimeofday(&timer, NULL);
	SET_TIMER(timer, (game->turn > 0) ? TIMEOUT : START_TIMEOUT);
	int tested = 0;

	Move best_move, move;
	float best_score, score;
	int found_at = -1;

	best_score = Montecarlo_try(&best_move, game);

	// We simulate here
	while (1) {

		// New random sol to test
		score = Montecarlo_try(&move, game);

		// Keep challenger if it's better
		if (score > best_score) {
			best_move = move;
			best_score = score;
			found_at = tested;
		}

		tested++;

		// No more time ?
#if LOCAL_INPUTS
		if (tested >= 150000) {
#else
		gettimeofday(&now, NULL);
		if (TIME_TO_STOP(timer, now)) {
#endif
			LOG_"Tested solutions: %d\n", tested);
			LOG_"Score: %.2f found at iteration %d\n", best_score, found_at);

			if (best_move.shoot) {
				game->input.shots++;
				printf("SHOOT %d\n", game->enemies[(int) best_move.val].id);
			} else {
				Point_move(&game->wolff, best_move.angle, best_move.val);
				printf("MOVE %.0f %.0f\n", game->wolff.x, game->wolff.y);
			}

			*choosen_move = best_move;
			Game_set_from_inputs(game);

			break;
		}
	}
}

inline bool Montecarlo_play_turn(Game *game, Move *move, float *score) {
	bool to_remove[MAX_DATA];
	memset(to_remove, 0, sizeof(bool) * MAX_DATA);

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
			to_remove[closest] = TRUE;
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

		game->shots++;

		/* 5- Kill my target if his life < 0 */
		if (game->enemies[eid].life <= 0) {
			memcpy(&game->enemies[eid], &game->enemies[eid+1], sizeof(Ennemy) * (game->ecount-1-eid));
			game->ecount--;

			// I get points
			*score += KILL_VALUE;
		}
	}

	/* 6- Enemies collect data points they share coordinates with */
	int i = 0;
	while (i < game->dcount) {
		if (to_remove[i]) {
			// Remove data from the data list
			memcpy(&game->data[i], &game->data[i+1], sizeof(Data) * (game->dcount-1-i));
			memcpy(&to_remove[i], &to_remove[i+1], sizeof(bool) * (game->dcount-1-i));
			game->dcount--;
		} else
			i++;
	}

	return TRUE;
}

inline float Montecarlo_try(Move *first_move, Game *game) {
	Move move;
	bool game_over = FALSE;
	float score = 0.0f;

	int i = 0;
	while(1) {
		game_over = !Montecarlo_play_turn(game, (i > 0) ? &move : first_move, &score);

		if (game_over) {
			Game_set_from_inputs(game);
			return -MAX_SCORE + i * 9999.0f;
		}

		// End of the game if no more data or no more enemies
		if (game->dcount <= 0 || game->ecount <= 0) {
			int total_life = 0;
			for (int e = 0; e < game->ecount; e++)
				total_life += game->enemies[e].life;

			for (int d=0; d < game->dcount; d++)
				score += DATA_VALUE;

			float bonus = FINAL_BONUS_SCORE(game->dcount, total_life, game->shots);
			Game_set_from_inputs(game);

			return score + bonus - i;
		}

		++i;
	}
}
