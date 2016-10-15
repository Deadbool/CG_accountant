#ifndef SRC_PARAMS_H_
#define SRC_PARAMS_H_

#include "Utils.h"

#pragma GCC optimize "O3,omit-frame-pointer,inline"

////////////////////////////
/////////// DEBUG //////////

#define LOG_INPUTS 1

#define LOG_SOLUTION 1

#define LOCAL_INPUTS 0

#define MC_ITERATIONS_FIX 0

#define TEST_CASE "simulation/tests/test_31.txt"

////////////////////////////

/**************************
****** CONTROL PANEL ******
**************************/
#define TIMEOUT 70000
#define START_TIMEOUT 600000

#define MAX_DEPTH 50

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


#endif /* SRC_PARAMS_H_ */
