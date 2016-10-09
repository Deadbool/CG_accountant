#!/bin/bash

cd $HOME/CodinGame/CG_accountant
mkdir -p CG_final_file

echo "/* This is the final source file for CodinGame */" > CG_final_file/tmp.c

echo "Merging params.h"
cat src/params.h >> CG_final_file/tmp.c

./include_order.sh

echo "Merging *.c"
cat src/*.c >> CG_final_file/tmp.c

echo "/* Merged by Deadbool */" >> CG_final_file/tmp.c

grep -v '^#include "' CG_final_file/tmp.c > CG_final_file/accountant.c
rm CG_final_file/tmp.c

echo "Compiling:"
gcc -O0 -g3 -Wall -fmessage-length=0 CG_final_file/accountant.c -o Debug/CG_accountant

#end