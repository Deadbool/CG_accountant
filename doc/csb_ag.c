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

/**************************
****** CONTROL PANEL ******
**************************/
#define MY_TIMEOUT 80000
#define HIS_TIMEOUT 55000

#define DEPTH 6
#define POPULATION_SIZE 51
#define TURNAMENT_SIZE 2

#define ANGLE_AMPLITUDE 32.0
#define THRUST_MIN_AMPLITUDE -40
#define THRUST_MAX_AMPLITUDE 250

#define MUTATION_PROB 0.03
#define SHIELD_PROB 0.4

#define TIMEOUT_LIMIT 30

/************************
 ****** EVAL COEFFS *****
 ************************/
#define COEFF_MY_RUNNER_ADVANCE 4.5
#define COEFF_MY_RUNNER_ANGLE_TO_CP 20.0
#define COEFF_HIS_RUNNER_ADVANCE 2.0

#define COEFF_MY_BLOCKER_DISTANCE_HIS_CP 0.9
#define COEFF_MY_BLOCKER_DISTANCE_HIS_RUNNER 0.5
#define COEFF_MY_BLOCKER_ANGLE_TO_HR 30.0

#define COEFF_TURN 80.0

#define SHIELD_PENALITY 850.0

//////////////////////////
///////// FLAGS //////////
#pragma GCC optimize "-O3"
//////////////////////////

//////////////////////////
///////// RAND ///////////
unsigned int g_seed = 12345678;
#define RAND_INT(max) (fast_rand()%(max))
#define BRAND_INT(min,max) ((min) + RAND_INT((max)-(min)))
#define RAND_DOUBLE() (((double) fast_rand()) / 0x7FFF)
#define BRAND_DOUBLE(min, max) ((min) + (RAND_DOUBLE()) * ((max)-(min)))
//////////////////////////

#define COND_LOG_ if (log) fprintf(stderr,
#define LOG_ fprintf(stderr,

typedef struct timeval timeval;
#define SET_TIMER(timer,timeout) do { timer.tv_usec += timeout;\
										if(timer.tv_usec >= 1000000){timer.tv_usec -= 1000000;timer.tv_sec++;}\
									 } while(0);
#define TIME_TO_STOP(timer, now) (timer.tv_sec == now.tv_sec && timer.tv_usec <= now.tv_usec)

#define PI 3.141592653589793
#define EPSILON 0.00001
#define DEG_TO_RAD(deg) (deg * PI / 180.0)
#define MAX_FITNESS 10000000.0

#define POD_COUNT 4
#define MAX_CHECKPOINTS 8
#define MAX_THRUST 200
#define MAX_ANGLE 18.0
#define MAP_WIDTH 16000
#define MAP_HEIGHT 9000

#define COLLISION_NONE 0
#define COLLISION_POD 1
#define COLLISION_CHECKPOINT 2

#define SHIELD -1
#define NEW_SHIELD 20

#define MY_POD_1 0
#define MY_POD_2 1
#define HIS_POD_1 2
#define HIS_POD_2 3

/********************
 ****** CLASSES *****
 ********************/
typedef struct {
	float x;
	float y;
}Point;

typedef struct { // Inherit of Point
	// inherited
	float x;
	float y;

	// own
	int id;
	float radius;
	float vx;
	float vy;
}Unit;

typedef struct { // Inherit of Unit
	// inherited
	float x;
	float y;
	int id;
	float radius;
	float vx;
	float vy;
}Checkpoint;

typedef struct Pod Pod;
struct Pod { // Inherit of Unit
	// inherited
	float x;
	float y;
	int id;
	float radius;
	float vx;
	float vy;

	// own
	float angle;
	int next_cp_id;
	int checked;
	int timeout;
	Pod *partner;
	int shield;
};

typedef struct {
	Unit *a;
	Unit *b;
	float t;
}Collision;

typedef struct {
	float angle; // -MAX_ANGLE to +MAX_ANGLE
	int thrust; // 0 to MAX_THRUST, or -1 for SHIELD
}Move;

typedef struct {
	Move moves_1[DEPTH];
	Move moves_2[DEPTH];
}Solution;

typedef struct {
	bool pod_1_collision;
	bool pod_2_collision;
	bool mb_col_hr;
}Report;

typedef struct {
	int turn;
	int current_lap;
	int laps;
	int cp_count;
	int win; // Cp checked to win

	Pod *inputs[POD_COUNT];
	Pod *pods[POD_COUNT];
	Checkpoint *checkpoints[MAX_CHECKPOINTS];

	Pod *my_runner;
	Pod *my_blocker;
	Pod *his_runner;
	Pod *his_blocker;
}Game;

typedef struct {
	Solution decision;
	Solution his_prediciton;

	// GA
	float scores[POPULATION_SIZE];
	Solution population[POPULATION_SIZE];
	Solution generation[POPULATION_SIZE];
}AI;

/***********************
 ****** PROTOTYPES *****
 ***********************/
int fast_rand();
float rand_angle();
int rand_thrust();

Point* Point_new(float x, float y);
float Point_distance2(Point *this, Point *p);
float Point_distance(Point *this, Point *p);
void Point_closest(Point *this, Point *a, Point *b, Point *closest);

bool Unit_collision(Unit *this, Unit *u, float current_time, Collision *collision);

Checkpoint* Checkpoint_new(int id);

Pod* Pod_new(int id);
float Pod_angle(Pod *this, Point *p);
float Pod_diff_angle(Pod *this, Point *p);
float Pod_dist2_from_trajectory(Pod *this, Point *_a, Point *_b, bool log);
void Pod_rotate(Pod *this, Point *p);
void Pod_boost(Pod *this, int thrust);
void Pod_move(Pod *this, float t);
void Pod_end(Pod *this);
void Pod_apply(Pod *this, Move *move, bool log);
void Pod_bounce(Pod *this, Pod *pod);
float Pod_score(Pod *this, Game *game);
void Pod_output(Pod *this, Move *move);

void Move_randomize(Move *this, int fact);

void Solution_randomize(Solution *this);
void Solution_create_child(Solution *parent_1, Solution *parent_2, Solution *child);
float Solution_score(Solution *my_solution, Solution *his_solution, Game *game, bool opponent_side, bool log);

void Report_init(Report *report);

void Game_get_inputs(Game *this, bool log);
void Game_init(Game *this, bool log);
void Game_set_from_inputs(Game *this);
void Game_new_turn(Game *this);
void Game_play_turn(Game *this, Report *report, bool opponent_side, bool log);

void AI_init(AI *this);
void AI_output_decision(AI *this, Game *game);
void AI_init_population(AI *this);
Solution* AI_play_turnament(AI *this);
void AI_predict_opponent(AI *this, Game *game);
void AI_make_decision(AI *this, Game *game);

/********************
 ****** METHODS *****
 ********************/
inline int fast_rand() {
	g_seed=(214013*g_seed+2531011);
	return ((g_seed>>16)&0x7FFF);
}

inline float rand_angle() {
	float a = BRAND_DOUBLE(-ANGLE_AMPLITUDE, ANGLE_AMPLITUDE);
	if (a < -MAX_ANGLE) a = -MAX_ANGLE;
	else if (a > MAX_ANGLE) a = MAX_ANGLE;
	return a;
}

inline int rand_thrust() {
	int t = BRAND_INT(THRUST_MIN_AMPLITUDE, THRUST_MAX_AMPLITUDE);
	if (t < 0) t = 0;
	else if (t > MAX_THRUST) t = MAX_THRUST;
	return t;
}

// === Point ===
#define SET_POINT(name,x_,y_) name.x = x_; name.y = y_;

Point* Point_new(float x, float y) {
	Point *new = (Point*) malloc(sizeof(Point));
	new->x = x;
	new->y = y;
	return new;
}

float Point_distance2(Point *this, Point *p) {
	return (this->x - p->x)*(this->x - p->x) + (this->y - p->y)*(this->y - p->y);
}

float Point_distance(Point *this, Point *p) {
	return sqrt(Point_distance2(this, p));
}

void Point_closest(Point *this, Point *a, Point *b, Point *closest) {
	float da = b->y - a->y;
	float db = a->x - b->x;
	float c1 = db*a->x + db*a->y;
	float c2 = -db*this->x + da*this->y;
	float det = da*da + db*db;

	if (det != 0) {
		closest->x = (da*c1 - db*c2) / det;
		closest->y = (da*c2 - db*c1) / det;
	} else {
		closest->x = this->x;
		closest->y = this->y;
	}
}

// === Collision ===
#define SET_COLLISION(name,a_, b_, t_) name.a = a_; name.b = b_; name.t = t_;

// === Unit ===
inline bool Unit_collision(Unit *this, Unit *u, float current_time, Collision *collision) {
	float x2 = this->x - u->x;
	float y2 = this->y - u->y;
	float r2 = this->radius + u->radius;
	float vx2 = this->vx - u->vx;
	float vy2 = this->vy - u->vy;

	float a = vx2*vx2 + vy2*vy2;

	if (a < EPSILON) {
		return NULL;
	} else {
		float b = 2.0*(x2*vx2 + y2*vy2);
		float c = x2*x2 + y2*y2 - r2*r2;
		float delta = b*b - 4.0*a*c;

		if (delta < 0) {
			return NULL;
		}

		float sqrtDelta = sqrt(delta);
		float d = 1.0/(2.0*a);
		float t1 = (-b + sqrtDelta)*d;
		float t2 = (-b - sqrtDelta)*d;
		float t = t1 < t2 ? t1 : t2;

		if (t < 0.0) {
			return NULL;
		}

		t += current_time;

		if (t > 1.0) {
			return false;
		}

		SET_COLLISION((*collision), this, u, t);
		return true;
	}

	return false;
}

// === Checkpoint ===
Checkpoint* Checkpoint_new(int id) {
	Checkpoint *new = (Checkpoint*) malloc(sizeof(Checkpoint));
	new->id = id;
	new->x = 0;
	new->y = 0;
	new->vx = 0;
	new->vy = 0;
	new->radius = 199;
	return new;
}

// === Pod ===
Pod* Pod_new(int id) {
	Pod *new = (Pod*) malloc(sizeof(Pod));
	new->id = id;
	new->x = 0;
	new->y = 0;
	new->vx = 0;
	new->vy = 0;
	new->radius = 400;
	new->angle = 0;
	new->partner = NULL;
	new->timeout = 100;
	new->checked = 0;
	new->next_cp_id = 1;
	new->shield = 0;
	return new;
}

inline float Pod_angle(Pod *this, Point *p) {
	float d = Point_distance((Point*) this, p);
	float dx = (p->x - this->x) / d;
	float dy = (p->y - this->y) / d;

	float a = acos(dx) * 180.0 / PI;

	if (dy < 0) a = 360.0 - a;

	return a;
}

inline float Pod_diff_angle(Pod *this, Point *p) {
	float a = Pod_angle(this, p);
	float right = this->angle <= a ? a - this->angle : 360.0 - this->angle + a;
	float left = this->angle >= a ? this->angle - a : this->angle + 360.0 - a;

	//LOG_"Diffangle=%.2f\n", a);

	return right < left ? right : -left;
}

inline float Pod_dist2_from_trajectory(Pod *this, Point *_a, Point *_b, bool log) {
	Point b, m;

	// Reference frame will be a
	if (_a->x <= _b->x) {
		SET_POINT(b, _b->x - _a->x, _b->y - _a->y);
		SET_POINT(m, this->x - _a->x, this->y - _a->y);
	} else {
		SET_POINT(b, _a->x - _b->x, _a->y - _b->y);
		SET_POINT(m, this->x - _b->x, this->y - _b->y);
	}

	float r = b.y / b.x;
	float n = abs(r * m.x - m.y);
	float res = (n*n) / (r*r + 1);

	COND_LOG_"Dist from traj: %.2f", sqrt(res));

	return res;
}

inline void Pod_rotate(Pod *this, Point *p) {
	float a = Pod_diff_angle(this, p);

	// Maximum rotation by turn is 18°
	if (a > MAX_ANGLE) a = MAX_ANGLE;
	else if (a < -MAX_ANGLE) a = -MAX_ANGLE;

	this->angle += a;

	if (this->angle >= 360.0) this->angle -= 360.0;
	else if (this->angle < 0.0) this->angle += 360.0;
}

inline void Pod_boost(Pod *this, int thrust) {
	if (thrust == SHIELD)
		this->shield = NEW_SHIELD;

	// A Pod which has activated shield cannot accelerate for 3 turns
	if (this->shield) return;

	float rad = DEG_TO_RAD(this->angle);
	this->vx += cos(rad) * thrust;
	this->vy += sin(rad) * thrust;
}

inline void Pod_move(Pod *this, float t) {
	this->x += this->vx * t;
	this->y += this->vy * t;
}

inline void Pod_end(Pod *this) {
	this->x = round(this->x);
	this->y = round(this->y);
	this->vx = (int) (this->vx * 0.85);
	this->vy = (int) (this->vy * 0.85);

	this->timeout -= 1;
	if (this->shield > 0)
		this->shield--;
}

inline void Pod_apply(Pod *this, Move *move, bool log) {
	// Rotate
	this->angle += move->angle;
	if (this->angle >= 360.0) this->angle -= 360.0;
	else if (this->angle < 0.0) this->angle += 360.0;
	//COND_LOG_"Pod turn of %.2f (%.2f) and boost of %d\n", move->angle, this->angle, move->thrust);

	// Boost
	Pod_boost(this, move->thrust);
}

inline void Pod_bounce(Pod *this, Pod *pod) {
	float m1 = this->shield ? 10 : 1;
	float m2 = pod->shield ? 10 : 1;
	float mcoeff = 640000 * (m1 + m2) / (m1 * m2);

	float nx = this->x - pod->x;
	float ny = this->y - pod->y;

	//float nxny2 = nx*nx + ny*ny; // 800² = 640000

	float d_vx = this->vx - pod->vx;
	float d_vy = this->vy - pod->vy;

	// fx and fy represent the impact vector
	float tmp = nx*d_vx + ny*d_vy;
	float fx = (nx * tmp) / mcoeff;
	float fy = (ny * tmp) / mcoeff;

	// Apply the impact vector once
	this->vx -= fx / m1;
	this->vy -= fy / m1;
	pod->vx += fx / m2;
	pod->vy += fy / m2;

	// If norm of the vector is < 120 normalize it to 120
	float impulse = sqrt(fx*fx + fy*fy);
	if (impulse < 120.0) {
		fx *= 120.0 / impulse;
		fy *= 120.0 / impulse;
	}

	// Apply the impact vector a second time
	this->vx -= fx / m1;
	this->vy -= fy / m1;
	pod->vx += fx / m2;
	pod->vy += fy / m2;
}

inline float Pod_score(Pod *this, Game *game) {
	if(this->checked == game->win-1) {
		return this->checked*50000
				- Point_distance((Point*) this,	(Point*) game->checkpoints[0]);
	} else if (this->checked == game->win) {
		return (this->vx+this->vy)*50000;
	} else {
		return this->checked*50000
				- Point_distance((Point*) this,	(Point*) game->checkpoints[this->next_cp_id]);
	}
}

void Pod_output(Pod *this, Move *move) {
	float a = this->angle + move->angle;

	if (a >= 360.0) a -= 360.0;
	else if (a < 0.0) a += 360.0;

	a = DEG_TO_RAD(a);
	float px = this->x + cos(a) * 10000.0;
	float py = this->y + sin(a) * 10000.0;

	if (move->thrust == SHIELD) {
		printf("%d %d SHIELD SHIELD\n", (int) round(px), (int) round(py));
		this->shield = NEW_SHIELD;
	} else
		printf("%d %d %d %d\n", (int) round(px), (int) round(py), move->thrust, move->thrust);
}

// === MOVE ===
inline void Move_randomize(Move *this, int fact) {
	if (fact <= 0)
		this->angle = rand_angle();

	if (fact >= 0)
		this->thrust= rand_thrust();

	if (RAND_DOUBLE() < SHIELD_PROB)
		this->thrust = (this->thrust == SHIELD) ? rand_thrust() : SHIELD;
}

// === Solution ===
inline void Solution_randomize(Solution *this) {
	int i;
	for (i=0; i < DEPTH; i++) {
		Move_randomize(&this->moves_1[i], 0);
		Move_randomize(&this->moves_2[i], 0);
	}
}

inline void Solution_create_child(Solution *parent_1, Solution *parent_2, Solution *child) {
	int c1 = RAND_INT(DEPTH);
	int c2 = RAND_INT(DEPTH);

	if (c2 < c1) {
		int t = c2;
		c2 = c1;
		c1 = t;
	}

	// Crossovers
	for (int i=0; i < c1; i++) {
		child->moves_1[i] = parent_1->moves_1[i];
		child->moves_2[i] = parent_1->moves_2[i];
	}

	for (int i=c1; i <= c2; i++) {
		child->moves_1[i] = parent_2->moves_1[i];
		child->moves_2[i] = parent_2->moves_2[i];
	}

	for (int i=c2; i < DEPTH; i++) {
		child->moves_1[i] = parent_1->moves_1[i];
		child->moves_2[i] = parent_1->moves_2[i];
	}

	// Mutations
	for (int i=0; i < DEPTH; i++) {
		if (RAND_DOUBLE() < MUTATION_PROB)
			Move_randomize(&child->moves_1[i], BRAND_INT(-1, 2));

		if (RAND_DOUBLE() < MUTATION_PROB)
			Move_randomize(&child->moves_2[i], BRAND_INT(-1, 2));
	}
}

inline float Solution_score(Solution *my_solution, Solution *his_solution, Game *game, bool opponent_side, bool log) {
	COND_LOG_"== New simulation ==\n");
	//int a;
	//float tj;

	// If we are opponent side, switch teams
	Pod *mr, *mb, *hr, *hb;
	if (opponent_side) {
		mr = game->his_runner;
		mb = game->his_blocker;
		hr = game->my_runner;
		hb = game->my_blocker;
	} else {
		mr = game->my_runner;
		mb = game->my_blocker;
		hr = game->his_runner;
		hb = game->his_blocker;
	}

	// TODO:: Improve evaluation
	float score = 0;
	int checked = mr->checked;
	Report report;

	// Play turns
	int i;
	for (i=0; i < DEPTH; i++) {
		/*COND_LOG_">> Turn %d\n", i);
		COND_LOG_"1# angle=%.0f thrust=%d\n", my_solution->moves_1[i].angle, my_solution->moves_1[i].thrust);
		COND_LOG_"2# angle=%.0f thrust=%d\n", my_solution->moves_2[i].angle, my_solution->moves_2[i].thrust);*/

		Pod_apply(game->pods[MY_POD_1], &my_solution->moves_1[i], log);
		Pod_apply(game->pods[MY_POD_2], &my_solution->moves_2[i], log);
		Pod_apply(game->pods[HIS_POD_1], &his_solution->moves_1[i], log);
		Pod_apply(game->pods[HIS_POD_2], &his_solution->moves_2[i], log);

		Game_play_turn(game, &report, opponent_side, log);

		if (mr->timeout <= 0 || mb->timeout <= 0 || hr->checked == game->win) {
			//COND_LOG_"Lose :'( --> runner_timeout=%d, blocker_timeout=%d\n", mr->timeout, mb->timeout);
			score = -MAX_FITNESS+i*10;
			break;
		} else if ((hr->timeout <= 0 || hb->timeout <= 0 || mr->checked == game->win)) {
			//COND_LOG_"Win ! :D\n");
			score = MAX_FITNESS-i*10;
			break;
		} else {
			// Runner
			score += COEFF_MY_RUNNER_ADVANCE * Pod_score(mr, game);
			score -= COEFF_HIS_RUNNER_ADVANCE * Pod_score(hr, game);
			score -= COEFF_MY_RUNNER_ANGLE_TO_CP *
						abs(Pod_diff_angle(mr, (Point*) game->checkpoints[mr->next_cp_id]));

			if (!opponent_side && mb->timeout < TIMEOUT_LIMIT &&
					(hr->timeout >= mb->timeout || hb->timeout >= mb->timeout)) {

				//COND_LOG_"TIMEOUT PROTECTION !\n");

				// Run to save from timeout
				score += COEFF_MY_RUNNER_ADVANCE * Pod_score(mb, game);

				/*a = abs(Pod_diff_angle(mb, (Point*) game->checkpoints[mb->next_cp_id])) + 180;
				if (a >= 360) a = 0;
				score -= COEFF_MY_RUNNER_ANGLE_TO_CP * a;*/

			} else {
				// Normal blocker
				score -= COEFF_MY_BLOCKER_DISTANCE_HIS_RUNNER * Point_distance((Point*) mb, (Point*) hr);
				score -= COEFF_MY_BLOCKER_DISTANCE_HIS_CP * Point_distance((Point*) mb, (Point*) game->checkpoints[hr->next_cp_id]);

				/*tj = Pod_dist2_from_trajectory(mb, (Point*) hr, (Point*) (game->checkpoints[hr->next_cp_id]), false);

				score -= tj;
				if (tj < 10000) {
					score -= COEFF_MY_BLOCKER_DISTANCE_HIS_RUNNER * Point_distance((Point*) mb, (Point*) hr);
				}

				score -= COEFF_MY_BLOCKER_ANGLE_TO_HR * abs(Pod_diff_angle(mb, (Point*) hr));*/
			}

			if (my_solution->moves_1[i].thrust == SHIELD) {
				score -= SHIELD_PENALITY;
				if (!report.pod_1_collision)
					my_solution->moves_1[i].thrust = rand_thrust();
			}

			if (my_solution->moves_2[i].thrust == SHIELD) {
				score -= SHIELD_PENALITY;
				if (!report.pod_2_collision)
					my_solution->moves_2[i].thrust = rand_thrust();
			}

			if (mr->checked > checked) {
				COND_LOG_"Checkpoint %d passed \n", mr->next_cp_id);
				score -= COEFF_TURN * i;
				checked++;
			}



		}

		//COND_LOG_"Runner checked: %d\n\n", mr->checked);
	}

	Game_set_from_inputs(game);

	return score;
}

// === Report ===
inline void Report_init(Report *this) {
	this->pod_1_collision = false;
	this->pod_2_collision = false;
	this->mb_col_hr = false;
}

// === Game ===
void Game_get_inputs(Game *this, bool log) {
	int last_cp_id;

	for (int i = 0; i < POD_COUNT; i++) {
		last_cp_id = this->inputs[i]->next_cp_id;

		scanf("%f%f%f%f%f%d",
				&this->inputs[i]->x,
				&this->inputs[i]->y,
				&this->inputs[i]->vx,
				&this->inputs[i]->vy,
				&this->inputs[i]->angle,
				&this->inputs[i]->next_cp_id);

		// In the loop to show log after the prompt of CG
		if (i == 0)
			COND_LOG_"\n=== INPUTS ===\n");

		COND_LOG_"** Pod %d **\n", i);

		COND_LOG_"x=%.0f y=%.0f\n", this->inputs[i]->x, this->inputs[i]->y);
		COND_LOG_"vx=%.0f vy=%.0f\n", this->inputs[i]->vx, this->inputs[i]->vy);
		COND_LOG_"a=%.0f\n", this->inputs[i]->angle);

		if (last_cp_id != this->inputs[i]->next_cp_id) {
			this->inputs[i]->checked++;
			this->inputs[i]->timeout = 100;
			this->inputs[i]->partner->timeout = 100;
		} else {
			this->inputs[i]->timeout--;
		}

		COND_LOG_"next_cp=%d checked=%d\n", this->inputs[i]->next_cp_id, this->inputs[i]->checked);

		if (this->inputs[i]->shield > 0) {
			this->pods[i]->shield--;
			COND_LOG_"SHIELD MALUS\n");
		}

		COND_LOG_"timeout=%d\n", this->inputs[i]->timeout);
	}
}

void Game_init(Game *this, bool log) {
	this->turn = 0;
	this->current_lap = 0;

	for (int i=0; i < POD_COUNT; i++) {
		this->pods[i] = Pod_new(i);
		this->inputs[i] = Pod_new(i);
	}

	this->pods[MY_POD_1]->partner = this->pods[MY_POD_2];
	this->pods[MY_POD_2]->partner = this->pods[MY_POD_1];
	this->pods[HIS_POD_1]->partner = this->pods[HIS_POD_2];
	this->pods[HIS_POD_2]->partner = this->pods[HIS_POD_1];

	this->inputs[MY_POD_1]->partner = this->inputs[MY_POD_2];
	this->inputs[MY_POD_2]->partner = this->inputs[MY_POD_1];
	this->inputs[HIS_POD_1]->partner = this->inputs[HIS_POD_2];
	this->inputs[HIS_POD_2]->partner = this->inputs[HIS_POD_1];

	scanf("%d", &this->laps);
	scanf("%d", &this->cp_count);
	this->win = (this->cp_count) * (this->laps);

	COND_LOG_"=== INIT GAME ===\n");
	COND_LOG_"Laps to do: %d [win at %d checked]\n", this->laps, this->win);

	COND_LOG_"** Checkpoints [%d] **\n", this->cp_count);

	for (int i=0; i < this->cp_count; i++) {
		this->checkpoints[i] = Checkpoint_new(i);
		scanf("%f%f", &this->checkpoints[i]->x, &this->checkpoints[i]->y);
		COND_LOG_"%d) x=%d y=%d\n", i, (int) this->checkpoints[i]->x, (int) this->checkpoints[i]->y);
	}
}

inline void Game_set_from_inputs(Game *this) {
	for (int i = 0; i < POD_COUNT; i++) {
		*this->pods[i] = *this->inputs[i];
	}
}

void Game_new_turn(Game *this) {
	this->turn++;
	Game_get_inputs(this, false);
	Game_set_from_inputs(this);

	if (Pod_score(this->pods[MY_POD_1], this) < Pod_score(this->pods[MY_POD_2], this)) {
		this->my_runner = this->pods[MY_POD_2];
		this->my_blocker = this->pods[MY_POD_1];
	} else {
		this->my_runner = this->pods[MY_POD_1];
		this->my_blocker = this->pods[MY_POD_2];
	}

	if (Pod_score(this->pods[HIS_POD_1], this) < Pod_score(this->pods[HIS_POD_2], this)) {
		this->his_runner = this->pods[HIS_POD_2];
		this->his_blocker = this->pods[HIS_POD_1];
	} else {
		this->his_runner = this->pods[HIS_POD_1];
		this->his_blocker = this->pods[HIS_POD_2];
	}
}

inline void Game_play_turn(Game *this, Report *report, bool opponent_side, bool log) {
	float t = 0.0;
	int i, j;
	bool prev_col_exist = false;
	int collision_type;
	Collision col, first_col, previous_col;

	// To suppress warnings
	previous_col.a = NULL;
	previous_col.b = NULL;
	first_col.b = NULL;
	first_col.b = NULL;

	Report_init(report);

	while (t < 1.0) {
		collision_type = COLLISION_NONE;

		// Looking for the collisions that are going to occur
		for (i=0; i < POD_COUNT; i++) {

			// Collision with a pod ?
			for (j=i+1; j < POD_COUNT; j++) {
				if(Unit_collision((Unit*) this->pods[i], (Unit*) this->pods[j], t, &col) &&
					    (collision_type == COLLISION_NONE || col.t < first_col.t) &&
				    	!(prev_col_exist && col.t == t && col.a == previous_col.a && col.b == previous_col.b)) {
					collision_type = COLLISION_POD;
					first_col = col;
				}
			}

			// Collision with a checkpoint ?
			if(Unit_collision((Unit*) this->pods[i], (Unit*) this->checkpoints[this->pods[i]->next_cp_id], t, &col) &&
			        (collision_type == COLLISION_NONE || col.t < first_col.t) &&
				    !(prev_col_exist && col.t == t && col.a == previous_col.a && col.b == previous_col.b)) {
				collision_type = COLLISION_CHECKPOINT;
				first_col = col;
			}
		}

		if (collision_type == COLLISION_NONE) {
			COND_LOG_"No more collision\n");

			// No collision, moving until end of turn
			for (i=0; i < POD_COUNT; i++) {
				Pod_move(this->pods[i], 1.0 - t);
				//COND_LOG_"Pod %d move to x=%d y=%d\n", i, (int) this->pods[i]->x, (int) this->pods[i]->y);
			}

			// End of the turn
			t = 1.0;
		} else {
			// Moving until time t of the collision
			for (i=0; i < POD_COUNT; i++) {
				Pod_move(this->pods[i], first_col.t - t);
				//COND_LOG_"Pod %d move to x=%d y=%d\n", i, (int) this->pods[i]->x, (int) this->pods[i]->y);
			}

			if (!opponent_side) {
				if ((Pod*) first_col.a == this->pods[MY_POD_1] && ((Pod*) first_col.b == this->his_runner || (Pod*) first_col.b == this->his_blocker))
					report->pod_1_collision = true;

				if ((Pod*) first_col.a == this->pods[MY_POD_2] && ((Pod*) first_col.b == this->his_runner || (Pod*) first_col.b == this->his_blocker))
					report->pod_2_collision = true;

				if ((Pod*) first_col.a == this->my_blocker && (Pod*) first_col.b == this->his_runner)
					report->mb_col_hr = true;
			}

			/*COND_LOG_"Collision between %d and %d at time %.3f\n",
									first_col.a->id, first_col.b->id, first_col.t);*/

			COND_LOG_"Collision between %d and %d at time %.3f [dist=%.2f max=%.0f]\n",
						first_col.a->id, first_col.b->id, first_col.t,
						Point_distance((Point*) first_col.a, (Point*) first_col.b),
						first_col.a->radius+first_col.b->radius);

			// Apply collision
			if (collision_type == COLLISION_CHECKPOINT) {
				((Pod*) first_col.a)->timeout = 100;
				((Pod*) first_col.a)->partner->timeout = 100;
				((Pod*) first_col.a)->checked++;
				((Pod*) first_col.a)->next_cp_id++;
				if (((Pod*) first_col.a)->next_cp_id >= this->cp_count) ((Pod*) first_col.a)->next_cp_id = 0;
			} else {
				Pod_bounce((Pod*) first_col.a, (Pod*) first_col.b);
			}

			prev_col_exist = true;
			previous_col = first_col;
			t = first_col.t;
		}
	}

	for (i=0; i < POD_COUNT; i++)
		Pod_end(this->pods[i]);
}

// === AI ===
void AI_init(AI *this) {
	Solution_randomize(&this->decision);
	Solution_randomize(&this->his_prediciton);
}

void AI_output_decision(AI *this, Game *game) {
	int winner_id = 0;
	for (int i=0; i < POPULATION_SIZE; i++) {
		this->scores[i] = Solution_score(&this->population[i], &this->his_prediciton, game, false, false);
		if (this->scores[i] > this->scores[winner_id])
			winner_id = i;
	}

	this->decision = this->population[winner_id];

	Pod_output(game->pods[MY_POD_1], &this->decision.moves_1[0]);
	Pod_output(game->pods[MY_POD_2], &this->decision.moves_2[0]);
}

inline void AI_init_population(AI *this) {
	// Taking the last decision and shift the first move
	this->population[0] = this->decision;
	for (int i=1; i < DEPTH; i++) {
		this->population[0].moves_1[i-1] = this->population[0].moves_1[i];
		this->population[0].moves_2[i-1] = this->population[0].moves_2[i];
	}

	for (int i=1; i < POPULATION_SIZE; i++)
		Solution_randomize(&this->population[i]);
}

inline Solution* AI_play_turnament(AI *this) {
	int id, winner_id;

	// Play
	winner_id = RAND_INT(POPULATION_SIZE);
	for(int j=1; j < TURNAMENT_SIZE; j++) {
		id = RAND_INT(POPULATION_SIZE);
		if (this->scores[id] > this->scores[winner_id])
			winner_id = id;
	}

	return &this->population[winner_id];
}

void AI_predict_opponent(AI *this, Game *game) {
	int tested = 0;
	timeval timer, now;

	gettimeofday(&timer, NULL);
	SET_TIMER(timer, HIS_TIMEOUT)

	// Taking the last decision and shift the first move
	this->population[0] = this->his_prediciton;
	for (int i=1; i < DEPTH; i++) {
		this->population[0].moves_1[i-1] = this->population[0].moves_1[i];
		this->population[0].moves_2[i-1] = this->population[0].moves_2[i];
	}

	// Fill with random
	for (int i=1; i < POPULATION_SIZE; i++)
		Solution_randomize(&this->population[i]);

	// We simulate here
	do {
		tested++;

		// Compute all the scores
		for (int i=0; i < POPULATION_SIZE; i++)
			this->scores[i] = Solution_score(&this->decision, &this->population[i], game, true, false);

		// Crossovers & Mutations
		for (int i=0; i < POPULATION_SIZE; i++)
			Solution_create_child(AI_play_turnament(this), AI_play_turnament(this), &this->generation[i]);

		// This new generation will be the population for next iteration
		memcpy(this->population, this->generation, POPULATION_SIZE * sizeof(Solution));

		gettimeofday(&now, NULL);
	} while (!TIME_TO_STOP(timer, now));

	LOG_"\nTested generations for him: %d\n", tested);

	int winner_id = 0;
	for (int i=0; i < POPULATION_SIZE; i++) {
		this->scores[i] = Solution_score(&this->decision, &this->population[i], game, true, false);
		if (this->scores[i] > this->scores[winner_id])
			winner_id = i;
	}

	this->his_prediciton = this->population[winner_id];

	//Solution_score(&this->his_prediction, game, true);
}

void AI_make_decision(AI *this, Game *game) {
	int tested = 0;
	//int counter = 0;
	//int winner_id;
	timeval timer, now;

	gettimeofday(&timer, NULL);
	SET_TIMER(timer, MY_TIMEOUT)

	AI_init_population(this);

	// We simulate here
	do {
		tested++;
		//counter++;

		// Compute all the scores
		for (int i=0; i < POPULATION_SIZE; i++)
			this->scores[i] = Solution_score(&this->population[i], &this->his_prediciton, game, false, false);

		// Crossovers & Mutations
		for (int i=0; i < POPULATION_SIZE; i++)
			Solution_create_child(AI_play_turnament(this), AI_play_turnament(this), &this->generation[i]);

		// This new generation will be the population for next iteration
		memcpy(this->population, this->generation, POPULATION_SIZE * sizeof(Solution));

#if 0
		if (counter == 30) {
			counter = 0;
			winner_id = 0;
			for (int i=0; i < POPULATION_SIZE; i++) {
				if (this->scores[i] > this->scores[winner_id])
					winner_id = i;
			}
			LOG_"%d --> fitness: %.2f\n", tested, this->scores[winner_id]);
		}
#endif

		gettimeofday(&now, NULL);
	} while (!TIME_TO_STOP(timer, now));

	LOG_"\nTested generations for me: %d\n", tested);

	//Solution_score(&this->decision, &this->his_prediciton, game, false, true);

	AI_output_decision(this, game);
}

/*******************
 ******* MAIN ******
 *******************/
int main() {
    Game game;
    AI brain;

    Game_init(&game, false);
    AI_init(&brain);

    // First turn just target next_cp with full thrust
    Game_get_inputs(&game, false);
    printf("%.0f %.0f BOOST\n", game.checkpoints[game.pods[MY_POD_1]->next_cp_id]->x,
    		game.checkpoints[game.pods[MY_POD_1]->next_cp_id]->y);
    printf("%.0f %.0f BOOST\n", game.checkpoints[game.pods[MY_POD_2]->next_cp_id]->x,
    		game.checkpoints[game.pods[MY_POD_2]->next_cp_id]->y);

    // game loop
	while (1) {
		Game_new_turn(&game);

		LOG_"My runner:   %d [checked=%d timeout=%d]\n", game.my_runner->id+1, game.my_runner->checked, game.my_runner->timeout);
		LOG_"My blocker:  %d [checked=%d timeout=%d]\n", game.my_blocker->id+1, game.my_blocker->checked, game.my_blocker->timeout);
		LOG_"His runner:  %d [checked=%d timeout=%d]\n", game.his_runner->id-1, game.his_runner->checked, game.his_runner->timeout);
		LOG_"His blocker: %d [checked=%d timeout=%d]\n", game.his_blocker->id-1, game.his_blocker->checked, game.his_blocker->timeout);
		//Pod_dist2_from_trajectory(game.my_blocker, (Point*) (game.checkpoints[game.his_runner->next_cp_id]), (Point*) (game.his_runner), true);

		AI_predict_opponent(&brain, &game);

		AI_make_decision(&brain, &game);
    }

    return EXIT_SUCCESS;
}
#endif
