#!/bin/bash

cd /home/nico/CodinGame/CG_accountant

echo "/* This is the final source file for CodinGame */" > CG_final_file/tmp.c

cat src/*.h >> CG_final_file/tmp.c
cat src/*.c >> CG_final_file/tmp.c

echo "/* Merged by Deadbool */" >> CG_final_file/tmp.c

echo "Final file merged."

grep -v '^#include "' CG_final_file/tmp.c > CG_final_file/accountant.c
rm CG_final_file/tmp.c

echo "Compiling:"
gcc -O0 -g3 -Wall -fmessage-length=0 CG_final_file/accountant.c -o Debug/CG_accountant

#end