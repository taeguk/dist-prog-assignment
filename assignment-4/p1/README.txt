Assignment #4 - Problem 1 [CUDA Programming 1 - Matrix Multiplication]
==============================================
    By 20141500 (taeguk. http://taeguk.me)

Usage
----------
./run.sh
(But, MATRIX_SIZE must be 32*N)

Description
--------------
Matrix Multiplication Program (C = A x B)

CUDA codes are optimized in CC 3.7 (Tesla K80)
CUDA_1 is a just simple cuda kernel.
CUDA_2 is optimized version. (using shared memory for B.)
CUDA_3 is more optimized version. (using shared memory for both A and B.)
