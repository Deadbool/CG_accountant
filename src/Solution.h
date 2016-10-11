#ifndef SRC_SOLUTION_H_
#define SRC_SOLUTION_H_

#include "params.h"
#include "Move.h"

typedef struct {
	Move moves[DEPTH];
}Solution;

void Solution_randomize(Solution *sol, int dcount);
void Solution_create_child(Solution *parent_1, Solution *parent_2, Solution *child, int ecount);

#endif /* SRC_SOLUTION_H_ */
