#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "collapse.h"

#define DEFAULT_INTEGER_RANGE 10000

int main(int argc, char *argv[])
{
    FILE *fp;
    int N;
    int integer_range = DEFAULT_INTEGER_RANGE;
    int result = 0;

    if (argc < 3) {
        printf("[Usage] %s <the number of integers> <output file name> <integer range : optional>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    fp = fopen(argv[2], "wt");
    if (argc >= 4)
        integer_range = atoi(argv[3]);

    srand(time(NULL));

    fprintf(fp, "%d\n", N);
    for(int i=0; i<N; ++i) {
        int rand_num = rand() % integer_range;
        result = ultimate_collapse(result + rand_num);
        fprintf(fp, "%d ", rand_num);
    }
    printf("Correct Result = %d\n", result);

    fclose(fp);

    return 0;
}