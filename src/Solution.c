#include "Solution.h"

inline void Solution_randomize(Solution *sol, int ecount) {
	for (int i=0; i < MAX_DEPTH; i++)
		Move_randomize(&sol->moves[i], ecount);
}

inline void Solution_create_child(Solution *parent_1, Solution *parent_2, Solution *child, int ecount) {
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


