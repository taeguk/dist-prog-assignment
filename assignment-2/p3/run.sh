#!/usr/bin/env bash

# default values
SERVER_PORT=19999
NUM_OF_THREADS=4

if [ ! -z "$1" ]
then
	SERVER_PORT=$1
fi

if [ ! -z "$2" ]
then
	NUM_OF_THREADS=$2
fi

echo "--- Run Web Server with $NUM_OF_THREADS worker threads. (port: $SERVER_PORT) ---"
gcc web_server.c request_queue.c http_lib.c -std=c99 -lpthread -o server.out &&
    ./server.out "$SERVER_PORT" "$NUM_OF_THREADS"