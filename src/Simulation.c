#include "Simulation.h"

inline void Simulation_play_turn(Game *game, Move *move) {
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

	/* 2- Move Wolff */
	if (!move->shoot)
		Point_move(&game->wolff, move->angle, move->val);

	/* 3- Am I dead ? */
	for (int e=0; e < game->ecount; e++) {
		if (Point_distance2(&game->wolff, &game->enemies[e].point) < ENNEMIES_RANGE_2) {
			LOG_">>> GAME OVER <<<\n");
			exit(0);
		}
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

	Simulation_output(game);
}

void Simulation_output(Game *game) {
	FILE *f = fopen("simulation/simu.txt", "w");
	if (f == NULL) {
		printf("Unable to open simu.txt file\n");
		exit(0);
	}

	// Wolff's position
	fprintf(f, "%.0f %.0f\n", game->wolff.x, game->wolff.y);

	// Data
	Data *d;
	fprintf(f, "%d\n", game->dcount);
	for (int i=0; i < game->dcount; i++) {
		d = &game->data[i];
		fprintf(f, "%d %.0f %.0f\n", d->id, d->point.x, d->point.y);
	}

	// Enemies
	Ennemy *e;
	fprintf(f, "%d\n", game->ecount);
	for (int i=0; i < game->ecount; i++) {
		e = &game->enemies[i];
		fprintf(f, "%d %.0f %.0f %d\n", e->id, e->point.x, e->point.y, e->life);
	}

	fclose(f);
}
