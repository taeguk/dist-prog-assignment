#!/usr/bin/env bash

echo -n "Input MATRIX_SIZE (32*N) >> "
read MATRIX_SIZE
echo

nvcc -arch=sm_37 -c cuda.cu -o cuda.o -DMATRIX_SIZE=$MATRIX_SIZE &&
gcc -fopenmp -c main.c -o main.o -DMATRIX_SIZE=$MATRIX_SIZE &&
gcc main.o cuda.o -o p1.out -lgomp -L/usr/local/cuda/lib64 -lcuda -lcudart &&
./p1.out
rm -rf *.o *.out
