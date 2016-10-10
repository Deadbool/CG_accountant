#if 0
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
typedef struct timeval timeval;

/**************************
****** CONTROL PANEL ******
**************************/

#define US_TIMEOUT 99000

#define MAX_MOVES 50
#define MAX_DISTANCE_TO_TRY 3000
#define MAX_RANDOM_MOVES 3

/**************************
**************************/

//////////////////////////
///////// FLAGS///////////

#pragma GCC optimize "-O3"

//////////////////////////

#define GRID_WIDTH 16000
#define GRID_HEIGHT 9000
#define MAX_ENTITIES 99

#define PI 3.141592653589793
#define RAND_ANGLE (randInt(359)+1)
#define TIME_TO_STOP(timer, now) (timer.tv_sec == now.tv_sec && timer.tv_usec <= now.tv_usec)
#define LOG_ if (log) fprintf(stderr,
#define SQRT_5 (2.2360679775)
#define FIBO(n) (( pow(1+SQRT_5, n) - (pow(1-SQRT_5, n)) ) / pow(2,n)*SQRT_5)

#define GAME_STATUS_RUNNING 0
#define GAME_STATUS_WIN 1
#define GAME_STATUS_LOSE -1

int randInt(int max) {
	double r;
	r = (rand()/(RAND_MAX*1.0)) * (max + 1);
	if (r > max)
		r = max;
	return (int) r;
}

// ===================
// ==== Structures ===
// ===================
typedef unsigned char status_t;
#define ALIVE 0
#define CRITICAL 1
#define DEAD 2

typedef struct {
	int x;
	int y;
	int move_size;
	status_t status;
}Entity;

typedef struct {
	int x;
	int y;
	int move_size;
	status_t status;
	Entity *target;
}Human;

typedef struct {
	int x;
	int y;
	int move_size;
	status_t status;
	int next_x;
	int next_y;
	Human *target;
}Zombie;

typedef struct {
	int player_x;
	int player_y;
	int human_count;
	int humans_x[MAX_ENTITIES];
	int humans_y[MAX_ENTITIES];
	int zombie_count;
	int zombies_x[MAX_ENTITIES];
	int zombies_y[MAX_ENTITIES];
	int zombies_next_x[MAX_ENTITIES];
	int zombies_next_y[MAX_ENTITIES];
}GameData;

typedef struct {
	int score;
	Human *player;
	int human_count;
	Human *humans[MAX_ENTITIES];
	int zombie_count;
	Zombie *zombies[MAX_ENTITIES];
}GameState;

// ===================
// ===== Entity ======
// ===================
int distance(Entity *arg0, Entity *arg1) {
	return (int) sqrt(pow(arg0->x - arg1->x, 2) + pow(arg0->y - arg1->y, 2));
}

int angleTo(Entity *e, Entity *target) {
	return (int) (180 * atan2(target->y - e->y, target->x - e->x) / PI);
}

void move(Entity *e, int angle, int dist) {
	if (dist > e->move_size)
		dist = e->move_size;

	double rad = angle * PI / 180;
	e->x = (int) (cos(rad) * dist + e->x);
	e->y = (int) (sin(rad) * dist + e->y);

	if (e->x < 0)
		e->x = 0;
	else if (e->x >= GRID_WIDTH)
		e->x = GRID_WIDTH-1;

	if (e->y < 0)
		e->y = 0;
	else if (e->y >= GRID_HEIGHT)
		e->y = GRID_HEIGHT-1;
}

// ===================
// ===== Human ======void
// ===================
Human *new_Human() {
	Human *new = malloc(sizeof(Human));
	new->move_size = 1000;
	new->status = ALIVE;
	return new;
}

// ===================
// ===== Zombie ======
// ===================
Zombie *new_Zombie() {
	Zombie *new = malloc(sizeof(Zombie));
	new->move_size = 400;
	new->status = ALIVE;
	return new;
}

void zombie_assignTarget(Zombie *z, GameState *gs) {
	int i, dist;
	Human *target = gs->player;
	int min_dist = distance((Entity*) z, (Entity*) gs->player);

	// Searching for the nearest human
	for (i = 0; i < gs->human_count; i++) {
		if (gs->humans[i]->status == ALIVE) {
			dist = distance((Entity*) z, (Entity*) gs->humans[i]);
			if (dist < min_dist) {
				target = gs->humans[i];
				min_dist = dist;
			}
		}
	}

	z->target = target;
	if (min_dist <= z->move_size) {
		z->next_x = z->target->x;
		z->next_y = z->target->y;
	} else {
		double limit = (1.0 * z->move_size)/min_dist;
		z->next_x = z->x + (int) ((z->target->x - z->x) * limit);
		z->next_y = z->y + (int) ((z->target->y - z->y) * limit);
	}
}

void zombie_move(Zombie *z, GameState *gs) {
	z->x = z->next_x;
	z->y = z->next_y;

	if (z->target != NULL) {
		if (z->x == z->target->x && z->y == z->target->y) {
			z->target->status = CRITICAL;
		} else {
			int dist = distance((Entity*) z, (Entity*) z->target);
			if (dist > z->move_size) {
				double limit = (1.0 * z->move_size)/dist;
				z->next_x = z->x + (int) ((z->target->x - z->x) * limit);
				z->next_y = z->y + (int) ((z->target->y - z->y) * limit);
			} else {
				z->next_x = z->target->x;
				z->next_y = z->target->y;
			}
		}
	}
}

void shuffle_zombies(Zombie *tab[], int len) {
	if (len > 1) {
		int i, j;
		Zombie *tmp;
		for (i = 0; i < len; i++) {
			j = i + rand() / (RAND_MAX / (len-i) + 1);
			tmp = tab[j];
			tab[j] = tab[i];
			tab[i] = tmp;
		}
	}
}

// =====================
// ===== GameState =====
// =====================

void get_GameData(GameData *gd) {
	int id_bin, i;

	// Ash inputs
	scanf("%d%d", &gd->player_x, &gd->player_y);

	// Humans inputs
	scanf("%d", &gd->human_count);
	for (i = 0; i < gd->human_count; i++) {
		scanf("%d%d%d", &id_bin, &gd->humans_x[i], &gd->humans_y[i]);
	}

	// Zombies inputs
	scanf("%d", &gd->zombie_count);
	for (i = 0; i < gd->zombie_count; i++) {
		scanf("%d%d%d%d%d", &id_bin, &gd->zombies_x[i], &gd->zombies_y[i], &gd->zombies_next_x[i], &gd->zombies_next_y[i]);
	}
}

GameState* create_GameState() {
	GameState *gs = malloc(sizeof(GameState));
	int i;

	// Memory allocation
	gs->score = 0;
	gs->player = new_Human();
	for (i = 0; i < MAX_ENTITIES; i++) {
		gs->humans[i] = new_Human();
		gs->zombies[i] = new_Zombie();
	}

	return gs;
}

void set_GameState(GameState *gs, GameData *gd) {
	int i;
	// Ash data
	gs->player->x = gd->player_x;
	gs->player->y = gd->player_y;

	// Humans data
	gs->human_count = gd->human_count;
	for (i = 0; i < gd->human_count; i++) {
		gs->humans[i]->x = gd->humans_x[i];
		gs->humans[i]->y = gd->humans_y[i];
		gs->humans[i]->status = ALIVE;
	}

	// Zombies data
	gs->zombie_count = gd->zombie_count;
	for (i = 0; i < gd->zombie_count; i++) {
		gs->zombies[i]->x = gd->zombies_x[i];
		gs->zombies[i]->y = gd->zombies_y[i];
		gs->zombies[i]->next_x = gd->zombies_next_x[i];
		gs->zombies[i]->next_y = gd->zombies_next_y[i];
		gs->zombies[i]->status = ALIVE;
		gs->zombies[i]->target = NULL;
	}
}

// ===================
// ===== Solution ====
// ===================
typedef struct {
	int rate;
	int played_turns;
	int x[MAX_MOVES];
	int y[MAX_MOVES];
}Solution;

Solution* new_Solution() {
	Solution *new = malloc(sizeof(Solution));
	new->rate = -1;
	return new;
}

void clone_Solution(Solution *dst, Solution *src) {
	dst->played_turns = src->played_turns;
	dst->rate = src->rate;
	memcpy(dst->x, src->x, MAX_MOVES);
	memcpy(dst->y, src->y, MAX_MOVES);
}

int solution_play_turn(int turn, int angle, int dist, Solution *s, GameState *gs, int *game_status, bool log) {
	int z_killed = 0;
	int score = 0;
	int h_alive = 0;
	int z_alive = 0;
	int z, h, i;

	LOG_"\n>>>>>>  Turn  <<<<<<\n");

	// 1) Zombies move
	LOG_"\nZombies move:\n");
	for (z = 0; z < gs->zombie_count; z++) {
		if (gs->zombies[z]->status != DEAD) {
			if (gs->zombies[z]->target == NULL) {
				LOG_" %d %d => %d %d targetting NOTHING\n", gs->zombies[z]->x, gs->zombies[z]->y, gs->zombies[z]->next_x, gs->zombies[z]->next_y);
			} else {
				LOG_" %d %d => %d %d targetting %d %d\n", gs->zombies[z]->x,
					gs->zombies[z]->y, gs->zombies[z]->next_x, gs->zombies[z]->next_y, gs->zombies[z]->target->x, gs->zombies[z]->target->y);
			}
			zombie_move(gs->zombies[z], gs);
		}
	}

	// 2) Ash moves
	LOG_"\nAsh moves:\n %d %d", gs->player->x, gs->player->y);
	move((Entity*) gs->player, angle, dist);
	s->x[turn] = gs->player->x;
	s->y[turn] = gs->player->y;
	LOG_" => %d %d\n", gs->player->x, gs->player->y);

	// 3) Ash kills zombies
	LOG_"\nAsh kills zombies:\n");
	for (z = 0; z < gs->zombie_count; z++) {
		if (gs->zombies[z]->status != DEAD && distance((Entity*) gs->player, (Entity*) gs->zombies[z]) < 2000) {
			LOG_" %d %d\n", gs->zombies[z]->x, gs->zombies[z]->y);
			gs->zombies[z]->status = DEAD;
			z_killed++;

			if (gs->zombies[z]->target != NULL && gs->zombies[z]->x == gs->zombies[z]->target->x && gs->zombies[z]->y == gs->zombies[z]->target->y)
				gs->zombies[z]->target->status = ALIVE;
		}
	}

	// 4) Zombies eat humans
	LOG_"\nZombies eat humans:\n");
	for (h = 0; h < gs->human_count; h++) {
		if (gs->humans[h]->status == CRITICAL) {
			LOG_" %d %d\n", gs->humans[h]->x, gs->humans[h]->y);
			gs->humans[h]->status = DEAD;
		} else if (gs->humans[h]->status == ALIVE)
			h_alive++;
	}

	/* Computing the score */
	for (i = 0; i < z_killed; i++)
		score += h_alive*h_alive*10 * FIBO(i+2);

	/* Re-assigning targets */
	for (z = 0; z < gs->zombie_count; z++) {
		if (gs->zombies[z]->status == ALIVE) {
			zombie_assignTarget(gs->zombies[z], gs);
			z_alive++;
		}
	}

	if (h_alive == 0)
		*game_status = GAME_STATUS_LOSE;
	else if (z_alive == 0)
		*game_status = GAME_STATUS_WIN;
	else
		*game_status = GAME_STATUS_RUNNING;

	LOG_"\n>> Score: %d\n", score);

	return score;
}

void generate_solution(Solution *s, GameState *gs) {
	int t, turn_score, rand_moves, z, game_status;
	s->played_turns = 0;
	s->rate = 0;
	gs->player->target = NULL;

	rand_moves = randInt(MAX_RANDOM_MOVES);

	z = 0;
	shuffle_zombies(gs->zombies, gs->zombie_count);

	for (t=0; t < MAX_MOVES; t++) {
		if (t < rand_moves) {
			turn_score = solution_play_turn(t, RAND_ANGLE, randInt(MAX_DISTANCE_TO_TRY), s, gs, &game_status, false);
		} else {
			while (gs->zombies[z]->status == DEAD) {
				z++;
				if (z >= gs->zombie_count)
					return; // Game is won
			}
			turn_score = solution_play_turn(t, angleTo((Entity*) gs->player,
					(Entity*) gs->zombies[z]), 1000, s, gs, &game_status, false);

		}

		if (game_status == GAME_STATUS_RUNNING) {
			s->rate += turn_score;
		} else if (game_status == GAME_STATUS_LOSE){
			s->rate = -1;
			return;
		} else {
			return;
		}
	}
}

void solution_validate(Solution *s, bool log) {
	fprintf(stderr, "Playing T%d of the solution", s->played_turns);
	printf("%d %d %d\n", s->x[s->played_turns], s->y[s->played_turns], s->rate);
	s->played_turns++;
}

// ===================
// ====== Game =======
// ===================
int main() {
	// Random initialisation
	srand(time(NULL));

	// Variables creation
	GameData inputs;
	GameState *game = create_GameState();
    timeval timer, now;
    int tested;

    Solution *action = new_Solution();
    Solution *challenger = new_Solution();

    // game loop
	int turns = 0;
	while (1) {
    	/********************
    	 ****** Inputs ******
    	 ********************/
		get_GameData(&inputs);
		set_GameState(game, &inputs);

		/********************
		 **** Simulation ****
		 ********************/
		tested = 0;

		// Setting the timer
		gettimeofday(&timer, NULL);
		if (turns > 0)
			timer.tv_usec += US_TIMEOUT;
		else {
			timer.tv_usec += US_TIMEOUT * 10;
		}
		if(timer.tv_usec >= 1000000) {
			timer.tv_usec -= 1000000;
			timer.tv_sec++;
		}

		// We simulate here
		do {
			generate_solution(challenger, game);
			set_GameState(game, &inputs);

			if (challenger->rate > action->rate)
				clone_Solution(action, challenger);

			tested++;
			gettimeofday(&now, NULL);
		} while (!TIME_TO_STOP(timer, now));

		turns++;
		fprintf(stderr, "\nTurn %d:\n", turns);
		fprintf(stderr, "Tested generations: %d\n",tested);

		solution_validate(action, true);
    }

    return EXIT_SUCCESS;
}
#endif
