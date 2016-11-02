#!/usr/bin/env bash

NUM_OF_PROCESS=36
NUM_OF_N=10000
DATA_FILE_NAME="data.txt"
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

echo "------- Make Random Integers -------"
gcc make_random_data.c collapse.c -std=c99 -o make_random_data.out && ./make_random_data.out "$NUM_OF_N" "$DATA_FILE_NAME"
echo
sleep 1

echo "------- p5_1.c -------"
eval "mpicc p5_1.c common_main.c collapse.c -o p5_1.out && time mpirun -np \"$NUM_OF_PROCESS\" -hostfile hosts ./p5_1.out \"$DATA_FILE_NAME\" $REDIRECTION"
echo
sleep 3

echo "------- p5_2.c -------"
eval "mpicc p5_2.c common_main.c collapse.c -o p5_2.out && time mpirun -np \"$NUM_OF_PROCESS\" -hostfile hosts ./p5_2.out \"$DATA_FILE_NAME\" $REDIRECTION"
echo
sleep 3

echo "------- p5_3.c -------"
eval "mpicc p5_3.c common_main.c collapse.c -o p5_3.out && time mpirun -np \"$NUM_OF_PROCESS\" -hostfile hosts ./p5_3.out \"$DATA_FILE_NAME\" $REDIRECTION"
echo
