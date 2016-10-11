#ifndef SRC_PARAMS_H_
#define SRC_PARAMS_H_

//#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

/**************************
****** CONTROL PANEL ******
**************************/
#define TIMEOUT 95000

#define DEPTH 15

#define POPULATION_SIZE 51
#define TURNAMENT_SIZE 2

#define MUTATION_PROB 0.03

#define MAP_W 16000
#define MAP_H 9000
#define MAX_DATA 100
#define MAX_ENNEMIES 100

#define WOLFF_STEP 1000.0f
#define ENNEMIES_RANGE 2000.0f
#define ENNEMIES_RANGE_2 4000000.0f
#define ENNEMIES_STEP 500.0f

#define DAMAGES(dist) (round((125000.0 * pow(dist, -1.2))))

#define DATA_VALUE 100.0f
#define KILL_VALUE 10.0f
#define FINAL_BONUS_SCORE(data, total_life, shots) (data * MAX(0.0f, (total_life - 3.0f*shots)) * 3.0f)

//////////////////////////
///////// RAND ///////////
unsigned int g_seed = 6320430;
int fast_rand();
inline int fast_rand() {
	g_seed=(214013*g_seed+2531011);
	return ((g_seed>>16)&0x7FFF);
}
#define RAND_INT(max) (fast_rand()%(max))
#define BRAND_INT(min,max) ((min) + RAND_INT((max)-(min)))
#define RAND_DOUBLE() (((double) fast_rand()) / 0x7FFF)
#define BRAND_DOUBLE(min, max) ((min) + (RAND_DOUBLE()) * ((max)-(min)))
//////////////////////////


// Utils
#define PI 3.141592653589793
#define EPSILON 0.00001f
#define DEG_TO_RAD(deg) (deg * PI / 180.0)
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

#endif /* SRC_PARAMS_H_ */
