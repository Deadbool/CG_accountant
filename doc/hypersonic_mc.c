#if 0
#pragma GCC optimize "O3,omit-frame-pointer,inline"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

/****** DEBUG ********/
#define LOCAL_INPUTS 0
//--------------------
#define LOG_INPUTS 0
//--------------------
#define LOG_SOLUTION 0
#define LOG_SIMULATION 0
//--------------------

/**************************
****** CONTROL PANEL******
**************************/
#define TIMEOUT 94000
#define DEPTH 13

// EVAL COEFFS
#define EVAL_PATIENCE_COEFF 0.9f
#define EVAL_DROP_BOMB_MALUS 30.0f
#define EVAL_BOX_COEFF 1000.0f
#define EVAL_GET_BOOST_B 70.0f
#define EVAL_GET_BOOST_R 50.0f
#define EVAL_ESCAPE_ENEMIES 50.0f

//////////////////////////
///////// RAND ///////////
unsigned int g_seed = 12345678;
#define RAND_INT(max) (fast_rand()%(max))
#define BRAND_INT(min,max) ((min) + RAND_INT((max)-(min)))
#define RAND_DOUBLE() (((double) fast_rand()) / 0x7FFF)
#define BRAND_DOUBLE(min, max) ((min) + (RAND_DOUBLE()) * ((max)-(min)))
#define RAND_MOVE() (RAND_INT(10))
int fast_rand();
inline int fast_rand() {
	g_seed=(214013*g_seed+2531011);
	return ((g_seed>>16)&0x7FFF);
}
//////////////////////////

// Utils
#define MAX_SCORE 10000000.0f
#define LOG_ fprintf(stderr,
typedef struct timeval timeval;
#define SET_TIMER(timer,timeout) do { timer.tv_usec += timeout;\
										if(timer.tv_usec >= 1000000){timer.tv_usec -= 1000000;timer.tv_sec++;}\
									 } while(0);
#define TIME_TO_STOP(timer, now) (timer.tv_sec == now.tv_sec && timer.tv_usec <= now.tv_usec)
#define MIN(a,b) (a <= b ? a : b)
#define MAX(a,b) (a >= b ? a : b)
typedef unsigned char bool;
#define TRUE 1
#define FALSE 0
#define NONE -1

/********************
 ****** DEFINES *****
 ********************/
// Game
#define MAX_PLAYERS 4
#define MAX_BOMBS 32
#define MAX_ITEMS 65

// Grid
#define GRID_W 13
#define GRID_H 11
#define VALID_POS(x,y) (x >= 0 && x < GRID_W && y >= 0 && y < GRID_H)
#define WRONG_POS(x,y) (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H)
#define EQUAL_POS(a,b) (a.x==b.x && a.y==b.y)

// Entity types
#define TYPE_PLAYER 0
#define TYPE_BOMB 1
#define TYPE_ITEM 2

// Moves
#define CENTER 0
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4
static const int DX[10] = {0, 0, 1, 0, -1};
static const int DY[10] = {0, -1, 0, 1, 0};
static const char DIRS[5] = {'C', 'U', 'R', 'D', 'L'};

/********************
 ****** CLASSES *****
 ********************/
typedef struct {
	int x;
	int y;
}Position;

typedef struct {
	Position pos;
	int id;
	int bombs;
	int range;

	int bboost;
	int rboost;
	int score;
	bool dead;
}Player;

typedef struct {
	Position pos;
	int owner;
	int cd;
	int range;
	bool dead;
}Bomb;

typedef struct {
	#define BONUS_RANGE 1
	#define BONUS_BOMB 2

	Position pos;
	int bonus;
	bool dead;
}Item;

typedef struct {
#define CELL_TYPE_EMPTY -1
#define CELL_TYPE_WALL -2
//		CELL_TYPE_BOX [0, 1, 2]
	int type;
	bool fire;

	int killers; // 4th first bits for each player

	/*int pcount;
	Player *p[MAX_PLAYERS];*/

	int bcount;
	Bomb *b[MAX_PLAYERS]; // bombs

	Item *i;
}Cell;

typedef struct {
	Cell cells[GRID_W][GRID_H];
}Grid;

typedef struct {
	Grid grid;
	int box_count;

	int pcount;
	Player players[MAX_PLAYERS];

	int bcount;
	Bomb bombs[MAX_BOMBS];

	int icount;
	Item items[MAX_ITEMS];
}Inputs;

typedef struct {
	int my_id;
	Grid grid;
	int box_count;

	int pcount;
	Player players[MAX_PLAYERS];
	Player *me;

	int bcount;
	Bomb bombs[MAX_BOMBS];

	int icount;
	Item items[MAX_ITEMS];

	Inputs inputs;
}Game;

typedef struct {
	int size;
	int moves[DEPTH]; // in [0:9] -> MOVE=[0:4] and BOMB=[5:9]
	bool bombs[DEPTH];
}Solution;

/***********************
 ****** PROTOTYPES *****
 ***********************/
int Position_distance(Position *a, Position *b);
int Position_distance_xy(Position *a, int x, int y);

void Player_output_move(Player *player, int move, bool bomb, float score);

void Grid_init(Grid *grid);
void Grid_log(Grid *grid, Game *game);

void Game_init(Game *game);
void Game_read_inputs(Game *game);
void Game_set_from_inputs(Game *game);
void Game_play_turn(Game *game, Solution *sol, int t);
void Game_log_turn(Game *game, Solution *sol, int t);

float Solution_score(Solution *sol, Game *game);
void Solution_log(Solution *sol, Game *game);

void monte_carlo(Game *game);
void test_simulation(Game *game);

/********************
 ****** METHODS *****
 ********************/
// === Positions ===
inline int Position_distance(Position *a, Position *b) {
	return abs(a->x - b->x) + abs(a->y - b->y);
}

inline int Position_distance_xy(Position *a, int x, int y) {
	return abs(a->x - x) + abs(a->y - y);
}

// === Player ===
inline void Player_output_move(Player *player, int move, bool bomb, float score) {
	int x = player->pos.x + DX[move];
	int y = player->pos.y + DY[move];

	if (bomb)
		printf("BOMB %d %d %.0f\n", x, y, score);
	else
		printf("MOVE %d %d %.0f\n", x, y, score);
}

// === Grid ===
void Grid_init(Grid *grid) {
	for (int x=0; x < GRID_W; x++) {
		for (int y=0; y < GRID_H; y++) {
			grid->cells[x][y].bcount = 0;
			grid->cells[x][y].fire = FALSE;
			grid->cells[x][y].killers = 0;
			grid->cells[x][y].i = NULL;
		}
	}
}

void Grid_log(Grid *grid, Game *game) {
	bool p;
	for (int y=0; y < GRID_H; y++) {
		for (int x=0; x < GRID_W; x++) {
			if (grid->cells[x][y].type > CELL_TYPE_EMPTY)
				LOG_"%d ", grid->cells[x][y].type);
			else {
				if (grid->cells[x][y].type == CELL_TYPE_WALL) {
					LOG_"X ");
				} else {
					p = FALSE;
					for (int i=0; i < game->pcount; i++) {
						if (!game->players[i].dead && game->players[i].pos.x==x && game->players[i].pos.y==y) {
							p = TRUE;
							break;
						}
					}

					if (grid->cells[x][y].bcount && !p)
						LOG_"@ ");
					else if (!grid->cells[x][y].bcount && p)
						LOG_"P ");
					else if (grid->cells[x][y].bcount && p)
						LOG_"& ");
					else
						LOG_". ");
				}
			}
		}
		LOG_"\n");
	}
	LOG_"\n");
}

// === Game ===
void Game_init(Game *game) {
	FILE *f = NULL;
	#if LOCAL_INPUTS
		f = fopen("init.txt", "r");
		if (f == NULL) {	game->pcount = 0;
		game->bcount = 0;
		game->icount = 0;
			printf("Unable to open input file\n");
			exit(0);
		}
	#else
		f = stdin;
	#endif

	int w, h;
	fscanf(f, "%d%d%d", &w, &h, &game->my_id);

#if LOG_INPUTS
	LOG_"%d %d %d\n", w, h, game->my_id);
#endif

	#if LOCAL_INPUTS
		fclose(f);
	#endif
}

void Game_read_inputs(Game *game) {
	Inputs *in = &game->inputs;
	FILE *f = NULL;
#if LOCAL_INPUTS
	f = fopen("inputs.txt", "r");
	if (f == NULL) {
		printf("Unable to open input file\n");
		exit(0);
	}
#else
	f = stdin;
#endif

	// Read grid
	Grid_init(&in->grid);
	in->box_count = 0;
	for (int y = 0; y < GRID_H; y++) {
		char row[GRID_W+1];
		fscanf(f ,"%s", row);

#if LOG_INPUTS
		LOG_"%s\n", row);
#endif

		for (int x=0; x < GRID_W; x++) {
			if (row[x]=='.') {
				in->grid.cells[x][y].type = CELL_TYPE_EMPTY;
			} else if (row[x]=='X') {
				in->grid.cells[x][y].type = CELL_TYPE_WALL;
			} else {
				in->grid.cells[x][y].type = row[x] - '0';
				in->box_count++;
			}
		}
	}

	// Read entities
	in->pcount = 0;
	in->bcount = 0;
	in->icount = 0;

	int n;
	fscanf(f, "%d", &n);
#if LOG_INPUTS
	LOG_"\n%d\n", n);
#endif

	// Suppose that all players are dead
	for (int i=0; i < MAX_PLAYERS; i++) {
		in->players[i].dead = TRUE;
	}

	int type;
	int owner;
	int x;
	int y;
	int p1;
	int p2;
	for (int i = 0; i < n; i++) {
		fscanf(f, "%d%d%d%d%d%d", &type, &owner, &x, &y, &p1, &p2);

#if LOG_INPUTS
		LOG_"%d %d %d %d %d %d\n", type, owner, x, y, p1, p2);
#endif

		switch(type) {
			case TYPE_PLAYER:
				in->players[owner].id = owner;
				in->players[owner].pos.x = x;
				in->players[owner].pos.y = y;
				in->players[owner].bombs = p1;
				in->players[owner].range = p2;
				in->players[owner].score = 0;
				in->players[owner].bboost = 0;
				in->players[owner].rboost = 0;
				in->players[owner].dead = FALSE;

				if (owner == game->my_id)
					game->me = &game->players[owner];

				in->pcount = MAX(in->pcount, owner+1);
				break;

			case TYPE_BOMB:
				in->bombs[in->bcount].owner = owner;
				in->bombs[in->bcount].pos.x = x;
				in->bombs[in->bcount].pos.y = y;
				in->bombs[in->bcount].cd = p1;
				in->bombs[in->bcount].range = p2;
				in->bombs[in->bcount].dead = FALSE;

				in->grid.cells[x][y].b[in->grid.cells[x][y].bcount] = &game->bombs[in->bcount];
				in->grid.cells[x][y].bcount++;

				in->bcount++;
				break;

			case TYPE_ITEM:
				in->items[in->icount].pos.x = x;
				in->items[in->icount].pos.y = y;
				in->items[in->icount].bonus = p1;
				in->items[in->icount].dead = FALSE;

				in->grid.cells[x][y].i = &game->items[in->icount];

				in->icount++;
				break;
		}
	}

#if LOG_INPUTS
	LOG_"---------------\n");
#endif

#if LOCAL_INPUTS
	fclose(f);
#endif
}

inline void Game_set_from_inputs(Game *game) {
	game->grid = game->inputs.grid;
	game->pcount = game->inputs.pcount;
	game->bcount = game->inputs.bcount;
	game->icount = game->inputs.icount;
	game->box_count = game->inputs.box_count;
	memcpy(game->players, game->inputs.players, sizeof(Player) * MAX_PLAYERS);
	memcpy(game->bombs, game->inputs.bombs, sizeof(Bomb) * MAX_BOMBS);
	memcpy(game->items, game->inputs.items, sizeof(Item) * MAX_ITEMS);
}

inline void Game_play_turn(Game *game, Solution *sol, int t) {
	/*** 1- Explosions ***/
	int todo = 0;
	Bomb *bqueue[MAX_BOMBS];
	int back_bombs[MAX_PLAYERS];
	memset(back_bombs, 0, sizeof(int) * MAX_PLAYERS);

	// Init the queue with bombs with a cd of 0
	for (int i=0; i < game->bcount; i++) {
		game->bombs[i].cd--;
		if (game->bombs[i].cd <= 0 && !game->bombs[i].dead) {
			bqueue[todo] = &game->bombs[i];
			todo++;
		}
	}

	// Treat all the bomb queue
	int curr = 0;
	int x,y;
	Bomb *bomb;
	Cell *cell;

	// Set fire to bombs' targets
	while (curr < todo) {
		bomb = bqueue[curr];
		game->grid.cells[bomb->pos.x][bomb->pos.y].fire = TRUE;
		back_bombs[bomb->owner]++;

		/* We don't need to activate bombs of the same cell because they always
		   have the same cd, so they are put in queue during init */

		// Treat all directions
		for (int d=1; d < 5; d++) {
			// In-range cells
			for (int n=1; n < bomb->range; n++) {
				x = bomb->pos.x + DX[d] * n;
				y = bomb->pos.y + DY[d] * n;

				if (WRONG_POS(x,y))
					break;

				cell = &game->grid.cells[x][y];

				if (cell->bcount) {
					if (!cell->fire) {
						cell->fire = TRUE;
						for (int i=0; i < cell->bcount; i++) {
							bqueue[todo] = cell->b[i];
							todo++;
						}
					}
					break;

				} else if (cell->type != CELL_TYPE_EMPTY || cell->i) {
					// There is a block that stop fire propagation
					if (cell->type >= 0) {
						// It's a  box
						cell->fire = TRUE;
						if ((cell->killers & (1<<bomb->owner)) == 0) {
							game->players[bomb->owner].score++;
							cell->killers |= (1<<bomb->owner);
						}
					}
					break;
				}

				// EMPTY with or without a player -> don't block
				cell->fire = TRUE;
			}
		}

		curr++;
	}

	/*** 2- Kill players and choose moves ***/
	int moves[MAX_PLAYERS];
	bool bombs[MAX_PLAYERS];

	int pmove_count;
	int possible_moves[5];
	possible_moves[0] = CENTER;

	for (int i=0; i < game->pcount; i++) {
		if (game->players[i].dead)
			continue;

		if (game->grid.cells[game->players[i].pos.x][game->players[i].pos.y].fire) {
			game->players[i].dead = TRUE;
			continue;
		}

		// Check all possible moves from current cell
		pmove_count = 1; // CENTER is always possible
		for (int m=1; m < 5; m++) {
			x = game->players[i].pos.x + DX[m];
			y = game->players[i].pos.y + DY[m];
			cell = &game->grid.cells[x][y];
			if (VALID_POS(x,y) && cell->type==CELL_TYPE_EMPTY && !cell->bcount) {
				possible_moves[pmove_count++] = m;
			}
		}

		if (i != game->my_id) { // TODO dummy bots for ennemies

			moves[i] = CENTER;
			bombs[i] = FALSE;

		} else { // TODO pseudo-ramdom heuristics

			// Choose one of these moves
			moves[i] = possible_moves[RAND_INT(pmove_count)];

			// Decide to bomb or not (if possible)
			if (game->grid.cells[game->players[i].pos.x][game->players[i].pos.y].bcount) {
				bombs[i] = FALSE;
			} else {
				bombs[i] = (game->players[i].bombs && RAND_DOUBLE() < 0.5) ? TRUE : FALSE;
			}
		}

		game->players[i].bombs += back_bombs[i];
	}

	/*** 3- Destroy cells and entities with fire ***/
	if (todo) {
		for (int x=0; x < GRID_W; x++) {
			for (int y=0; y < GRID_H; y++) {
				cell = &game->grid.cells[x][y];

				if (cell->fire) {
					cell->fire = FALSE;

					if (cell->type > CELL_TYPE_EMPTY) { // if it's a box

						if (cell->type) {
							// There is a bonus inside
							game->items[game->icount].pos.x = x;
							game->items[game->icount].pos.y = y;
							game->items[game->icount].bonus = cell->type;
							cell->i = &game->items[game->icount];
							game->icount++;
						}

						cell->type = CELL_TYPE_EMPTY;
						cell->killers = 0;
						game->box_count--;

					} else {
						// We are sure that it's not a wall (wall cannot be fired)
						if (cell->i) {
							cell->i = NULL;
						} else {
							for (int i=0; i < cell->bcount; i++) {
								cell->b[i]->dead = TRUE;
							}
							cell->bcount = 0;
						}
					}
				}
			}
		}
	}

	/*** 4- Apply moves ***/
	Player *player;
	for (int i=0; i < game->pcount; i++) {
		if (game->players[i].dead)
			continue;

		player = &game->players[i];

		// If it's a BOMB move, drop a bomb
		if (bombs[i]) {
			Bomb *bomb = &game->bombs[game->bcount];
			game->bcount++;
			bomb->dead = FALSE;
			bomb->owner = i;
			bomb->cd = 8;
			bomb->range = player->range;
			bomb->pos.x = player->pos.x;
			bomb->pos.y = player->pos.y;
			player->bombs--;

			cell = &game->grid.cells[player->pos.x][player->pos.y];
			cell->b[cell->bcount] = bomb;
			cell->bcount++;
		}

		// Move
		player->pos.x += DX[moves[i]];
		player->pos.y += DY[moves[i]];
		cell = &game->grid.cells[player->pos.x][player->pos.y];

		// Get the potential item
		if (cell->i) {
			if (cell->i->bonus == BONUS_BOMB) {
				player->bombs++;
				player->bboost++;
			} else {
				player->range++;
				player->rboost++;
			}

			cell->i->dead = TRUE;
			cell->i = NULL;
		}

		// If it's my move -> update solution
		if (i == game->my_id) {
			sol->moves[t] = moves[i];
			sol->bombs[t] = bombs[i];
		}
	}
}

void Game_log_turn(Game *game, Solution *sol, int t) {
	/*** 1- Explosions ***/
	int todo = 0;
	Bomb *bqueue[MAX_BOMBS];
	int back_bombs[MAX_PLAYERS];
	memset(back_bombs, 0, sizeof(int) * MAX_PLAYERS);

	// Init the queue with bombs with a cd of 0
	for (int i=0; i < game->bcount; i++) {
		game->bombs[i].cd--;
		if (game->bombs[i].cd <= 0 && !game->bombs[i].dead) {
			bqueue[todo] = &game->bombs[i];
			todo++;
		}
	}

	// Treat all the bomb queue
	int curr = 0;
	int x,y;
	Bomb *bomb;
	Cell *cell;

	// Set fire to bombs' targets
	while (curr < todo) {
		bomb = bqueue[curr];
		game->grid.cells[bomb->pos.x][bomb->pos.y].fire = TRUE;
		back_bombs[bomb->owner]++;

		/* We don't need to activate bombs of the same cell because they always
		   have the same cd, so they are put in queue during init */

		// Treat all directions
		for (int d=1; d < 5; d++) {
			// In-range cells
			for (int n=1; n < bomb->range; n++) {
				x = bomb->pos.x + DX[d] * n;
				y = bomb->pos.y + DY[d] * n;

				if (WRONG_POS(x,y))
					break;

				cell = &game->grid.cells[x][y];

				if (cell->bcount) {
					if (!cell->fire) {
						cell->fire = TRUE;
						for (int i=0; i < cell->bcount; i++) {
							bqueue[todo] = cell->b[i];
							todo++;
						}
					}
					break;

				} else if (cell->type != CELL_TYPE_EMPTY || cell->i) {
					// There is a block that stop fire propagation
					if (cell->type >= 0) {
						// It's a  box
						cell->fire = TRUE;
						if ((cell->killers & (1<<bomb->owner)) == 0) {
							game->players[bomb->owner].score++;
							cell->killers |= (1<<bomb->owner);
						}
					}
					break;
				}

				// EMPTY with or without a player -> don't block
				cell->fire = TRUE;
			}
		}

		curr++;
	}

	/*** 2- Kill players and choose moves ***/
	int moves[MAX_PLAYERS];
	bool bombs[MAX_PLAYERS];

	for (int i=0; i < game->pcount; i++) {
		if (game->players[i].dead)
			continue;

		if (game->grid.cells[game->players[i].pos.x][game->players[i].pos.y].fire) {
			game->players[i].dead = TRUE;
			continue;
		}

		if (i != game->my_id) {

			moves[i] = CENTER;
			bombs[i] = FALSE;

		} else {
			moves[i] = sol->moves[t];
			bombs[i] = sol->bombs[t];
		}

		game->players[i].bombs += back_bombs[i];
	}

	/*** 3- Destroy cells and entities with fire ***/
	if (todo) {
		for (int x=0; x < GRID_W; x++) {
			for (int y=0; y < GRID_H; y++) {
				cell = &game->grid.cells[x][y];

				if (cell->fire) {
					cell->fire = FALSE;

					if (cell->type > CELL_TYPE_EMPTY) { // if it's a box

						if (cell->type) {
							// There is a bonus inside
							game->items[game->icount].pos.x = x;
							game->items[game->icount].pos.y = y;
							game->items[game->icount].bonus = cell->type;
							cell->i = &game->items[game->icount];
							game->icount++;
						}

						cell->type = CELL_TYPE_EMPTY;
						cell->killers = 0;
						game->box_count--;

					} else {
						// We are sure that it's not a wall (wall cannot be fired)
						if (cell->i) {
							cell->i = NULL;
						} else {
							for (int i=0; i < cell->bcount; i++) {
								cell->b[i]->dead = TRUE;
							}
							cell->bcount = 0;
						}
					}
				}
			}
		}
	}

	/*** 4- Apply moves ***/
	Player *player;
	for (int i=0; i < game->pcount; i++) {
		if (game->players[i].dead)
			continue;

		player = &game->players[i];

		// If it's a BOMB move, drop a bomb
		if (bombs[i]) {
			Bomb *bomb = &game->bombs[game->bcount];
			game->bcount++;
			bomb->dead = FALSE;
			bomb->owner = i;
			bomb->cd = 8;
			bomb->range = player->range;
			bomb->pos.x = player->pos.x;
			bomb->pos.y = player->pos.y;
			player->bombs--;

			cell = &game->grid.cells[player->pos.x][player->pos.y];
			cell->b[cell->bcount] = bomb;
			cell->bcount++;
		}

		// Move
		player->pos.x += DX[moves[i]];
		player->pos.y += DY[moves[i]];
		cell = &game->grid.cells[player->pos.x][player->pos.y];

		// Get the potential item
		if (cell->i) {
			if (cell->i->bonus == BONUS_BOMB) {
				player->bombs++;
				player->bboost++;
			} else {
				player->range++;
				player->rboost++;
			}

			cell->i->dead = TRUE;
			cell->i = NULL;
		}
	}
}

void Game_log(Game *game) {
	for (int i=0; i < game->pcount; i++) {
		if (!game->players[i].dead)
			LOG_"P%d: x=%d y=%d score=%d\n", i, game->players[i].pos.x, game->players[i].pos.y, game->players[i].score);
	}

	Grid_log(&game->grid, game);
}

// === Solution ===
inline float Solution_score(Solution *sol, Game *game) {
	float score = 0.0f;
	float turn_score;
	float patience = 1.0f;
	sol->size = 0;
	Player *p;
	int d;

	/*********** EVAL ***********/	//TODO

	for (int t=0; t < DEPTH; t++) {
		turn_score = 0.0f;

		// Simulate the turn
		Game_play_turn(game, sol, t);

		// That's lost :'(
		if (game->me->dead) {
			Game_set_from_inputs(game);
			return -MAX_SCORE + 10.0f * t;
		}

		sol->size++;

		// Try to escape enemies as we can to not be trapped
		for (int i=0; i < game->pcount; i++) {
			if (i != game->my_id) {
				p = &game->players[i];
				d = Position_distance(&p->pos, &game->me->pos);
				if (d <= 3 || ((p->pos.x==game->me->pos.x || p->pos.y==game->me->pos.y) && d <= 5))
					turn_score -= EVAL_ESCAPE_ENEMIES;
			}
		}

		// Destroy boxes as early as possible
		turn_score += game->me->score * EVAL_BOX_COEFF;
		game->me->score = 0;

		// Malus the bomb every turn
		score -= sol->bombs[t] * EVAL_DROP_BOMB_MALUS;

		// Get the boosts
		turn_score += game->me->rboost * EVAL_GET_BOOST_R;
		turn_score += game->me->bboost * EVAL_GET_BOOST_B;
		game->me->bboost = 0;
		game->me->rboost = 0;

		// Early turns are more important
		score += turn_score * patience;
		patience *= EVAL_PATIENCE_COEFF;
	}

	// Reset intial game state
	Game_set_from_inputs(game);

	return score;
}

void Solution_log(Solution *sol, Game *game) {
	for (int i=0; i < DEPTH; i++) {
		Game_log_turn(game, sol, i);
		LOG_"Move: %c:%d\n", DIRS[sol->moves[i]], sol->bombs[i]);
		Game_log(game);
	}
	Game_set_from_inputs(game);
}

/*******************
 **** ALGORITHMS ***
 *******************/
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
#if LOG_SOLUTION
			LOG_"Solution:\n");
			for (int m=0; m < best_sol.size; m++)
				LOG_"t%d -> [%c]%d:%d\n", m, DIRS[best_sol.moves[m]], best_sol.moves[m], best_sol.bombs[m]);
			LOG_"\n");
#endif
#if LOG_SIMULATION
			Solution_log(&best_sol, game);
#endif
			Player_output_move(game->me, best_sol.moves[0], best_sol.bombs[0], best_score);
			break;
		}
	}
}

void test_simulation(Game *game) {
	timeval timer, now;
	gettimeofday(&timer, NULL);
	SET_TIMER(timer, TIMEOUT)
	int tested = 0;

	Solution sol;
	int timer_step = 1000;

	// We simulate here
	while (1) {
		tested += timer_step;

		for (int i=0; i < timer_step; i++) {
			Game_play_turn(game, &sol, 0);
			Game_set_from_inputs(game);
		}

		// No more time ?
		gettimeofday(&now, NULL);
		if TIME_TO_STOP(timer, now) {
			break;
		}
	}

	LOG_"\nTested generations: %d\n", tested);

	switch(game->my_id) {
		case 0:printf("MOVE 0 0\n");break;
		case 1:printf("MOVE 12 10\n");break;
		case 2:printf("MOVE 12 0\n");break;
		case 3:printf("MOVE 0 10\n");break;
	}
}

/*******************
 ******* MAIN ******
 *******************/
int main() {
	Game game;
	Game_init(&game);

    // game loop
	while (1) {

		Game_read_inputs(&game);
		Game_set_from_inputs(&game);

		monte_carlo(&game);

		// Local solution test
		/*Solution s;
		s.moves[0]  = 3; s.bombs[0]  = 0;
		s.moves[1]  = 3; s.bombs[1]  = 0;
		s.moves[2]  = 1; s.bombs[2]  = 1;
		s.moves[3]  = 1; s.bombs[3]  = 0;
		s.moves[4]  = 3; s.bombs[4]  = 0;
		s.moves[5]  = 2; s.bombs[5]  = 0;
		s.moves[6]  = 2; s.bombs[6]  = 0;
		s.moves[7]  = 1; s.bombs[7]  = 0;
		s.moves[8]  = 2; s.bombs[8]  = 0;
		s.moves[9]  = 4; s.bombs[9]  = 0;
		s.moves[10] = 0; s.bombs[10] = 0;
		s.moves[11] = 0; s.bombs[11] = 0;
		Solution_log(&s, &game);*/

#if LOCAL_INPUTS
		break;
#endif
	}

    return EXIT_SUCCESS;
}
#endif
