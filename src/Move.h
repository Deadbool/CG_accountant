/*
 * Move.h
 *
 *  Created on: 10 oct. 2016
 *      Author: nico
 */

#ifndef SRC_MOVE_H_
#define SRC_MOVE_H_

#include "params.h"

typedef struct {
	bool shoot;
	float val;   // (shoot) ? target_id : distance
	float angle; // only used if !shoot
}Move;

#endif /* SRC_MOVE_H_ */
