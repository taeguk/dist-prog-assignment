#!/usr/bin/env bash

NUM_OF_PROCESS=36

if [ ! -z "$1" ]
then
	NUM_OF_PROCESS=$1
fi

INPUT_FILE_PATH="example/large/falls_1.ppm"
if [ ! -z "$2" ]
then
	INPUT_FILE_PATH=$2
fi


OUTPUT_FILE_PATH="output/seq.ppm"

echo "------- Sequential -------"
gcc sequential.c ppm.c -o sequential.out -std=c99 && time ./sequential.out "$INPUT_FILE_PATH" "$OUTPUT_FILE_PATH"
echo
sleep 1

OUTPUT_FILE_PATH="output/par.ppm"

echo "------- Parallel -------"
echo "* Run with $NUM_OF_PROCESS processes."
echo
mpicc -std=c99 ppm.c parallel.c -o parallel.out && time mpirun -np "$NUM_OF_PROCESS" -hostfile hosts ./parallel.out "$INPUT_FILE_PATH" "$OUTPUT_FILE_PATH"
echo