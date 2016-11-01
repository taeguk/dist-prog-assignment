#!/usr/bin/env bash

NUM_OF_PROCESS=100
#REDIRECTION="> /dev/null"

if [ ! -z "$1" ]
then
	NUM_OF_PROCESS=$1
fi

echo "------- Sequential -------"
gcc sequential.c ppm.c -o sequential.out -std=c99 && time ./sequential.out
echo

#echo "------- Parallel -------"
#echo "* Run with $NUM_OF_PROCESS processes."
#echo
#eval "mpicc -std=c99 ppm.c parallel.c -o parallel && time mpirun -np $NUM_OF_PROCESS ./parallel $NUM_OF_N $REDIRECTION"
#echo
