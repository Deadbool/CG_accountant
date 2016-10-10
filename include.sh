#!/bin/bash

# Choose here the right order to cat .h files

FILES=(

	Point
	Enemy
	Data
	Move
	Solution
	Game
)

for f in "${FILES[@]}"
do
	echo "Merging $f.h"
	cat src/$f.h >> CG_final_file/tmp.c
done

#end