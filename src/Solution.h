#ifndef SRC_SOLUTION_H_
#define SRC_SOLUTION_H_

#include "params.h"
#include "Move.h"

typedef struct {
	int size;
	Move moves[DEPTH];
}Solution;

#endif /* SRC_SOLUTION_H_ */
