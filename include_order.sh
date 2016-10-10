#!/bin/bash

# Choose here the right order to cat .h files

echo "Merging Point.h"
cat src/Point.h >> CG_final_file/tmp.c

echo "Merging Unit.h"
cat src/Unit.h >> CG_final_file/tmp.c

echo "Merging Game.h"
cat src/Game.h >> CG_final_file/tmp.c

#end