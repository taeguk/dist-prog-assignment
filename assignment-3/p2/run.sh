#!/usr/bin/env bash

echo -n "Input the number of threads >> "
read THREAD_NUM
echo

#echo -n "Input the number of Tasks >> "
#read N
#echo -n "Input Max Size of Tasks >> "
#read M
#echo

echo "-------- OpenMP example2 Program With My Scheduler--------"
g++ example2_myschd.cpp -fopenmp -o example2_myschd.out && GOMP_CPU_AFFINITY="2 3 0 1" \
    bash -c "./example2_myschd.out $THREAD_NUM $N $M"
echo

echo "-------- OpenMP example2 Program With My Static Scheduler (chunk size 1) --------"
g++ example2_mystatic.cpp -fopenmp -o example2_mystatic.out && GOMP_CPU_AFFINITY="2 3 0 1" \
    bash -c "./example2_mystatic.out $THREAD_NUM 1"
echo

echo "-------- OpenMP example2 Program With My Static Scheduler (max chunk size) --------"
g++ example2_mystatic.cpp -fopenmp -o example2_mystatic.out && GOMP_CPU_AFFINITY="2 3 0 1" \
    bash -c "./example2_mystatic.out $THREAD_NUM 0"
echo

echo "-------- OpenMP example2 Program With OMP_SCHEDULE=static --------"
g++ example2_omp.cpp -fopenmp -o example2_omp.out && OMP_SCHEDULE=static \
    bash -c "echo \$OMP_SCHEDULE; ./example2_omp.out $THREAD_NUM $N $M"
echo

echo "-------- OpenMP example2 Program With OMP_SCHEDULE=dynamic --------"
g++ example2_omp.cpp -fopenmp -o example2_omp.out && OMP_SCHEDULE=dynamic \
    bash -c "echo \$OMP_SCHEDULE; ./example2_omp.out $THREAD_NUM $N $M"
echo

echo "-------- OpenMP example2 Program With OMP_SCHEDULE=guided --------"
g++ example2_omp.cpp -fopenmp -o example2_omp.out && OMP_SCHEDULE=guided \
    bash -c "echo \$OMP_SCHEDULE; ./example2_omp.out $THREAD_NUM $N $M"
echo