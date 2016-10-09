/* This is the final source file for CodinGame */
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
#define TIMEOUT 82000

#define DEPTH 6
#define POPULATION_SIZE 51
#define TURNAMENT_SIZE 2

#define MUTATION_PROB 0.03

//////////////////////////
///////// RAND ///////////
unsigned int g_seed = 6320430;
#define RAND_INT(max) (fast_rand()%(max))
#define BRAND_INT(min,max) ((min) + RAND_INT((max)-(min)))
#define RAND_DOUBLE() (((double) fast_rand()) / 0x7FFF)
#define BRAND_DOUBLE(min, max) ((min) + (RAND_DOUBLE()) * ((max)-(min)))
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
#ifndef SRC_GAME_H_
#define SRC_GAME_H_

typedef struct {
	int turn;
}Game;

void Game_init(Game *game);


#endif /* SRC_GAME_H_ */

void Game_init(Game *game) {
	game->turn = 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main()
{
	Game game;

	Game_init(&game);

    // game loop
    while (1) {
    	game.turn++;
    	LOG_"=== Turn %d ===\n", game.turn);

        int x;
        int y;
        scanf("%d%d", &x, &y);
        int dataCount;
        scanf("%d", &dataCount);
        for (int i = 0; i < dataCount; i++) {
            int dataId;
            int dataX;
            int dataY;
            scanf("%d%d%d", &dataId, &dataX, &dataY);
        }
        int enemyCount;
        scanf("%d", &enemyCount);
        for (int i = 0; i < enemyCount; i++) {
            int enemyId;
            int enemyX;
            int enemyY;
            int enemyLife;
            scanf("%d%d%d%d", &enemyId, &enemyX, &enemyY, &enemyLife);
        }

        // Write an action using printf(). DON'T FORGET THE TRAILING \n
        // To debug: fprintf(stderr, "Debug messages...\n");

        printf("MOVE 8000 4500\n"); // MOVE x y or SHOOT id
    }

    return 0;
}
/* Merged by Deadbool */
