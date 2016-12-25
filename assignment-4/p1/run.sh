#!/usr/bin/env bash

echo -n "Input Block Width >> "
read BLOCK_WIDTH
echo -n "Input Block Height >> "
read BLOCK_HEIGHT

if [ -z $BLOCK_WIDTH ]; then
    BLOCK_WIDTH=32
fi
if [ -z $BLOCK_HEIGHT ]; then
    BLOCK_HEIGHT=32
fi

echo -n "Input Matrix Size ($BLOCK_WIDTH*N and $BLOCK_HEIGHT*M) >> "
read MATRIX_SIZE
echo

if [ -z $MATRIX_SIZE ]; then
    MATRIX_SIZE=1024
fi

echo "Block Width  = $BLOCK_WIDTH"
echo "Block Height  = $BLOCK_HEIGHT"
echo "Matrix Size  = $MATRIX_SIZE"

nvcc -arch=sm_37 -c cuda.cu -o cuda.o -DMATRIX_SIZE=$MATRIX_SIZE -DBLOCK_WIDTH=$BLOCK_WIDTH -DBLOCK_HEIGHT=$BLOCK_HEIGHT &&
gcc -fopenmp -c main.c -o main.o -DMATRIX_SIZE=$MATRIX_SIZE -DBLOCK_WIDTH=$BLOCK_WIDTH -DBLOCK_HEIGHT=$BLOCK_HEIGHT &&
gcc main.o cuda.o -o p1.out -lgomp -L/usr/local/cuda/lib64 -lcuda -lcudart &&
./p1.out
rm -rf *.o *.out
