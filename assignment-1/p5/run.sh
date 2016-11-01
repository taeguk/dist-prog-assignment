#!/usr/bin/env bash

NUM_OF_PROCESS=100
NUM_OF_N=10000
#REDIRECTION="> /dev/null"

if [ ! -z "$1" ]
then
	NUM_OF_PROCESS=$1
fi

if [ ! -z "$2" ]
then 
	NUM_OF_N=$2
fi

echo "* Run with $NUM_OF_PROCESS processes."
echo "* Run with $NUM_OF_N integers."
echo

echo "------- p5_1.c -------"
eval "mpicc p5_1.c -o p5_1.out && time mpirun -np $NUM_OF_PROCESS ./p5_1.out $NUM_OF_N $REDIRECTION"
echo

echo "------- p5_2.c -------"
eval "mpicc p5_2.c -o p5_2.out && time mpirun -np $NUM_OF_PROCESS ./p5_2.out $NUM_OF_N $REDIRECTION"
echo

echo "------- p5_3.c -------"
eval "mpicc p5_3.c -o p5_3.out && time mpirun -np $NUM_OF_PROCESS ./p5_3.out $NUM_OF_N $REDIRECTION"
echo
