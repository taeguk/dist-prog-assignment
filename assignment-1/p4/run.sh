#!/usr/bin/env bash

NUM_OF_PROCESS=36
REDIRECTION="" #"> /dev/null"

if [ ! -z "$1" ]
then
	NUM_OF_PROCESS=$1
fi

echo "* Run with $NUM_OF_PROCESS processes."
echo

echo "------- Using MPI_Scan -------"
eval "mpicc mpi_scan.c -o mpi_scan.out && time mpirun -np \"$NUM_OF_PROCESS\" -hostfile hosts ./mpi_scan.out $REDIRECTION"
echo
sleep 3

echo "------- Using My Solution -------"
eval "mpicc my_solution.c -o my_solution.out && time mpirun -np \"$NUM_OF_PROCESS\" -hostfile hosts ./my_solution.out $REDIRECTION"
echo