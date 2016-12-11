#!/usr/bin/env bash

echo -n "Input the number of threads >> "
read THREAD_NUM
echo

#echo -n "Input the number of Tasks >> "
#read N
#echo -n "Input Max Size of Tasks >> "
#read M
#echo

echo "-------- OpenMP example Program With My Scheduler--------"
g++ example_myschd.cpp -fopenmp -o example_myschd.out && GOMP_CPU_AFFINITY="2 3 0 1" \
    bash -c "./example_myschd.out $THREAD_NUM $N $M"
echo

echo "-------- OpenMP example Program With OMP_SCHEDULE=static --------"
g++ example_omp.cpp -fopenmp -o example_omp.out && OMP_SCHEDULE=static \
    bash -c "echo \$OMP_SCHEDULE; ./example_omp.out $THREAD_NUM $N $M"
echo

echo "-------- OpenMP example Program With OMP_SCHEDULE=dynamic --------"
g++ example_omp.cpp -fopenmp -o example_omp.out && OMP_SCHEDULE=dynamic \
    bash -c "echo \$OMP_SCHEDULE; ./example_omp.out $THREAD_NUM $N $M"
echo

echo "-------- OpenMP example Program With OMP_SCHEDULE=guided --------"
g++ example_omp.cpp -fopenmp -o example_omp.out && OMP_SCHEDULE=guided \
    bash -c "echo \$OMP_SCHEDULE; ./example_omp.out $THREAD_NUM $N $M"
echo