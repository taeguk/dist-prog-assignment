#!/usr/bin/env bash

# default values
SERVER_IP="127.0.0.1"
SERVER_PORT=19999
NUM_OF_THREADS=4
NUM_OF_ACCESSES=1
REQUEST_PATHS=(
    "/path1"
    "/path2"
    "/path3"
    )

if [ ! -z "$1" ]
then
	NUM_OF_THREADS=$1
fi

if [ ! -z "$2" ]
then
	NUM_OF_ACCESSES=$2
fi

if [ ! -z "$3" ]
then
	SERVER_PORT=$3
fi

echo "--- Run Client Simulator with $NUM_OF_THREADS threads and $NUM_OF_ACCESSES accesses per a thread. ---"
gcc client_simulator.c -std=c99 -lpthread -lrt -o client.out &&
    ./client.out "$SERVER_IP" "$SERVER_PORT" "$NUM_OF_THREADS" "$NUM_OF_ACCESSES" \
        "${#REQUEST_PATHS[@]}" ${REQUEST_PATHS[@]}