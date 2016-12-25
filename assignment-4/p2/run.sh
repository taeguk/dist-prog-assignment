#!/usr/bin/env bash

echo -n "Input N >> "
read N
echo

nvcc -arch=sm_37 -c cuda.cu -o cuda.o -DN=$N &&
gcc -fopenmp -c main.c -o main.o -DN=$N &&
gcc main.o cuda.o -o p2.out -lgomp -L/usr/local/cuda/lib64 -lcuda -lcudart -lstdc++ &&
./p2.out
rm -rf *.o *.out
