#!/usr/bin/env bash

echo -n "Input the number of threads >> "
read THREAD_NUM
echo

echo -n "How many keys should be inserted in the main thread? "
read IN_1
echo -n "How many ops total should be executed? "
read IN_2
echo -n "Percent of ops that should be searches? (between 0 and 1) "
read IN_3
echo -n "Percent of ops that should be inserts? (between 0 and 1) "
read IN_4
echo

echo "-------- Pthread Reader Writer Lock --------"
gcc my_rand.c pth_ll_rwl.c -lpthread -o pthread.out
cmd='echo "$IN_1" "$IN_2" "$IN_3" "$IN_4" | ./pthread.out "$THREAD_NUM"'
echo "$cmd"
eval "$cmd"
echo

echo "-------- My Reader Writer Lock --------"
gcc my_rand.c my_pth_ll_rwl.c my_rwl.c -lpthread -o my.out
cmd='echo "$IN_1" "$IN_2" "$IN_3" "$IN_4" | ./my.out "$THREAD_NUM"'
echo "$cmd"
eval "$cmd"
echo