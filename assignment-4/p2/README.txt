Assignment #4 - Problem 1 [CUDA Programming 2 - Reduction]
==============================================
    By 20141500 (taeguk. http://taeguk.me)

Usage
----------
./run.sh

Description
--------------
Max Reduction Program

CUDA codes are optimized in CC 3.7 (Tesla K80)
CUDA_1 is a just simple reduction. (path divergence problem)
CUDA_2 is optimized version. (no path divergence)
CUDA_3 is more optimized version. (using shared memory, bank conflict problem)
CUDA_4 is more and more optimized version. (no bank conflict)

Reference
--------------
http://developer.download.nvidia.com/compute/cuda/1.1-Beta/x86_website/projects/reduction/doc/reduction.pdf
