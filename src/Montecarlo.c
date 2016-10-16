#include "Montecarlo.h"

float monte_carlo(Game *game, Solution *choosen_sol, float choosen_score) {
	timeval timer, now;
	gettimeofday(&timer, NULL);
	SET_TIMER(timer, (game->turn > 0) ? TIMEOUT : START_TIMEOUT);
	int tested = 0;

	Solution best_sol, sol;
	float best_score, score;
	int found_at = -1;

	best_sol = *choosen_sol;
	best_score = choosen_score;

	// We simulate here
	while (1) {

		/*if (tested == 9188) {
			int bk = 0;
		}*/

		// New random sol to test
		score = Montecarlo_try(&sol, game);

		// Keep challenger if it's better
		if (score > best_score) {
			best_sol = sol;
			best_score = score;
			found_at = tested;
		}

		tested++;

		// No more time ?
	#if MC_ITERATIONS_FIX > 0
		if (tested >= MC_ITERATIONS_FIX) {
	#else
		gettimeofday(&now, NULL);
		if (TIME_TO_STOP(timer, now)) {
	#endif
			LOG_"Tested solutions: %d\n", tested);
			LOG_"Score: %.2f found at iteration %d\n", best_score, found_at);
			LOG_"Shots: %d\n", game->input.shots);

			*choosen_sol = best_sol;

			return best_score;
		}
	}
}

inline bool Montecarlo_play_turn(Game *game, Move *move, float *score) {
	int to_remove[MAX_DATA];
	int will_kill[MAX_ENNEMIES];
	memset(to_remove, 0, sizeof(int) * MAX_DATA);

	bool run_away = FALSE;
	int damages[MAX_ENNEMIES];

	float shoot_prob = 0.5;
	int must_target_size = 0;
	int must_target[MAX_ENNEMIES];

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
			to_remove[closest]++;
			will_kill[e] = closest;
		} else {
			will_kill[e] = -1;
		}

		// Must shoot low life enemies
		dist = Point_distance(&game->wolff, &game->enemies[e].point);
		if (dist <= ENNEMIES_RANGE) {
			run_away = TRUE;
		} else {
			damages[e] = DAMAGES(dist);
			if (game->enemies[e].life <= damages[e]) {
				shoot_prob = 0.75;
				must_target[must_target_size++] = e;
			}
		}
	}

	/* 2- Determine my move */ // TODO Heuristics for decision helping
	if (!run_away && RAND_DOUBLE() < shoot_prob) {
		move->shoot = TRUE;
		move->val = (must_target_size && RAND_DOUBLE() < 0.8) ?
				must_target[RAND_INT(must_target_size)] : RAND_INT(game->ecount);
	} else {
		move->shoot = FALSE;
		move->val = RAND_DIST();
		if (move->val > WOLFF_STEP)
			move->val = WOLFF_STEP;

		move->angle = (RAND_DOUBLE() < 0.15) ?
				Point_angle_to(&game->wolff, &game->data[RAND_INT(game->dcount)].point)
				: RAND_ANGLE();

		Point_move(&game->wolff, move->angle, move->val);
	}

	/* 3- Am I dead ? */
	for (int e=0; e < game->ecount; e++) {
		// Wolff is static so we just have to verify the gap between him and enemies
		if (Point_distance2(&game->wolff, &game->enemies[e].point) <= ENNEMIES_RANGE_2)
			return FALSE;
	}

	/* 4- Should I shoot or not ?*/
	if (move->shoot) {
		int eid = (int) move->val;
		game->enemies[eid].life -= damages[eid];

		game->shots++;

		/* 5- Kill my target if his life < 0 */
		if (game->enemies[eid].life <= 0) {
			if (will_kill[eid] >= 0)
				to_remove[will_kill[eid]]--;
			memmove(&game->enemies[eid], &game->enemies[eid+1], sizeof(Ennemy) * (game->ecount-1-eid));
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
			memmove(&game->data[i], &game->data[i+1], sizeof(Data) * (game->dcount-1-i));
			memmove(&to_remove[i], &to_remove[i+1], sizeof(int) * (game->dcount-1-i));
			game->dcount--;
		} else
			i++;
	}

	return TRUE;
}

inline float Montecarlo_try(Solution *sol, Game *game) {
	bool game_over = FALSE;
	float score = game->input.score;
	sol->size = 0;

	for (int t=0; t < MAX_DEPTH; t++) {
		game_over = !Montecarlo_play_turn(game, &sol->moves[t], &score);
		sol->size++;

		if (game_over) {
			Game_set_from_inputs(game);
			return -MAX_SCORE + t * 9999.0f;
		}

		// End of the game if no more data or no more enemies
		if (game->dcount <= 0 || game->ecount <= 0) {
			for (int d=0; d < game->dcount; d++)
				score += DATA_VALUE;

			float bonus = FINAL_BONUS_SCORE(game->dcount, game->input.total_life, game->shots);

			Game_set_from_inputs(game);

			return score + bonus;
		}
	}

	return -MAX_SCORE - 10.0f;
}
