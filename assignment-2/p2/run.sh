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
gcc my_rand.c pth_ll_rwl.c -DOUTPUT -lpthread -o pthread.out
cmd="echo $IN_1 $IN_2 $IN_3 $IN_4 | ./pthread.out $THREAD_NUM"
echo "$cmd"
eval "$cmd"
echo

echo "-------- My Simple Reader Writer Lock --------"
gcc my_rand.c my_simple_pth_ll_rwl.c my_simple_rwl.c -DOUTPUT -lpthread -o my_simple.out
cmd="echo $IN_1 $IN_2 $IN_3 $IN_4 | ./my_simple.out $THREAD_NUM"
echo "$cmd"
eval "$cmd"
echo

echo "-------- My Optimized Reader Writer Lock --------"
gcc my_rand.c my_optimized_pth_ll_rwl.c my_optimized_rwl.c -DOUTPUT -lpthread -o my_optimized.out
cmd="echo $IN_1 $IN_2 $IN_3 $IN_4 | ./my_optimized.out $THREAD_NUM"
echo "$cmd"
eval "$cmd"
echo

echo "-------- Diff between pthread's and my simple results. --------"
diff pthread_result.txt my_simple_result.txt

echo "-------- Diff among pthread's and my optimized results. --------"
diff pthread_result.txt my_optimized_result.txt