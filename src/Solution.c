#include "Solution.h"

inline void Solution_randomize(Solution *sol, int dcount) {
	for (int i=0; i < DEPTH; i++)
		Move_randomize(&sol->moves[i], dcount);
}

inline void Solution_create_child(Solution *parent_1, Solution *parent_2, Solution *child, int ecount) {
	int c1 = RAND_INT(DEPTH);
	int c2 = RAND_INT(DEPTH);

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

	for (int i=c2; i < DEPTH; i++) {
		if (RAND_DOUBLE() < MUTATION_PROB)
			Move_randomize(&child->moves[i], ecount);
		else
			child->moves[i] = parent_1->moves[i];
	}
}


