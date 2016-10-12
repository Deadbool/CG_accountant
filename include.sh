#!/bin/bash

# Choose here the right order to cat .h files

FILES=(

	Utils
	Params
	Point
	Enemy
	Data
	Game
	Move
	Solution
	Simulation
	Montecarlo
)

for f in "${FILES[@]}"
do
	echo "Merging $f.h"
	cat src/$f.h >> CG_final_file/tmp.c
done

#end