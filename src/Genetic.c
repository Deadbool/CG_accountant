#include "Genetic.h"

inline void Genetic_create_child(Solution *parent_1, Solution *parent_2, Solution *child, int ecount) {
	int size = MIN(parent_1->size, parent_2->size);
	int c1 = RAND_INT(size);
	int c2 = RAND_INT(size);

	if (c2 < c1) {
		int t = c2;
		c2 = c1;
		c1 = t;
	}

	// Crossovers
	for (int i=0; i < c1; i++) {
		if (RAND_DOUBLE() < MUTATION_PROB)
			Move_randomize(&child->moves[i], ecount);
		else
			child->moves[i] = parent_1->moves[i];
	}

	for (int i=c1; i <= c2; i++) {
		if (RAND_DOUBLE() < MUTATION_PROB)
			Move_randomize(&child->moves[i], ecount);
		else
			child->moves[i] = parent_2->moves[i];
	}

	for (int i=c2; i < size; i++) {
		if (RAND_DOUBLE() < MUTATION_PROB)
			Move_randomize(&child->moves[i], ecount);
		else
			child->moves[i] = parent_1->moves[i];
	}

	child->size = size;
}
#if 0
float genetic(Game *game, Solution *choosen_sol, float choosen_score) {
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
		score = Genetic_try(&sol, game);

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
			LOG_"Score: %.6f found at iteration %d\n", best_score, found_at);
			LOG_"Shots: %d\n", game->input.shots);

			*choosen_sol = best_sol;

			return best_score;
		}
	}
}

inline bool Genetic_play_turn(Game *game, Move *move, float *score) {
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

	/* 2- Determine my move */ // TODO Heuristics for decision helping
	if (RAND_DOUBLE() < 0.5) {
		move->shoot = TRUE;
		move->val = RAND_INT(game->ecount);
	} else {
		move->shoot = FALSE;
		move->val = RAND_DIST();
		if (move->val > WOLFF_STEP)
			move->val = WOLFF_STEP;
		move->angle = RAND_ANGLE();

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
		game->enemies[eid].life -= DAMAGES(Point_distance(&game->wolff, &game->enemies[eid].point));

		game->shots++;

		/* 5- Kill my target if his life < 0 */
		if (game->enemies[eid].life <= 0) {
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
			memmove(&to_remove[i], &to_remove[i+1], sizeof(bool) * (game->dcount-1-i));
			game->dcount--;
		} else
			i++;
	}

	return TRUE;
}

inline float Genetic_try(Solution *sol, Game *game) {
	bool game_over = FALSE;
	float score = game->input.score;
	sol->size = 0;

	for (int t=0; t < MAX_DEPTH; t++) {
		game_over = !Genetic_play_turn(game, &sol->moves[t], &score);
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

#endif
