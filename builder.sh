#!/bin/bash

cd $HOME/CodinGame/CG_accountant
mkdir -p CG_final_file

DATE=$(date +%d/%m/%Y)
TIME=$(date +%H:%M)

echo "/*** Merged by Deadbool the $DATE at $TIME ***/" > CG_final_file/tmp.c
echo "" >> CG_final_file/tmp.c

./include.sh

echo "Merging *.c"
cat src/*.c >> CG_final_file/tmp.c

grep -v '^#include "' CG_final_file/tmp.c > CG_final_file/accountant.c
rm CG_final_file/tmp.c

echo "Compiling."
gcc -O0 -g3 -Wall -fmessage-length=0 CG_final_file/accountant.c -o Debug/CG_accountant -lm

#end