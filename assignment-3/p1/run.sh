#!/usr/bin/env bash

echo -n "Input the number of threads >> "
read THREAD_NUM
echo

skip_cnt=0
if [ ! -z "$1" ]
then
    skip_cnt=$1
fi

if [ $skip_cnt -lt 1 ]
then
    echo "-------- Serial Program Using Balanced Binary Search Tree (STL Set) --------"
    g++ bst_serial.cpp -o bst_serial.out && ./bst_serial.out
    echo
fi

if [ $skip_cnt -lt 2 ]
then
    echo "-------- Parallel Program Using Balanced Binary Search Tree (STL Set) --------"
    g++ bst_parallel.cpp -fopenmp -o bst_parallel.out && ./bst_parallel.out "$THREAD_NUM"
    echo
fi

if [ $skip_cnt -lt 3 ]
then
    echo "-------- Serial Program Using STL Vector --------"
    g++ vec_serial.cpp -o vec_serial.out && ./vec_serial.out
    echo
fi

if [ $skip_cnt -lt 4 ]
then
    echo "-------- Parallel Program Using STL Vector --------"
    g++ vec_parallel.cpp -fopenmp -o vec_parallel.out && ./vec_parallel.out "$THREAD_NUM"
    echo
fi

if [ $skip_cnt -lt 5 ]
then
    echo "-------- Optimized Parallel Program Using STL Vector --------"
    g++ vec_parallel_optimized.cpp -fopenmp -o vec_parallel_optimized.out && ./vec_parallel_optimized.out "$THREAD_NUM"
    echo
fi