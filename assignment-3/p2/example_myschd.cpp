#include <complex>
#include <cstdio>
#include <cstdlib>
#include <omp.h>

typedef std::complex<double> complex;

int MandelbrotCalculate(complex c, int maxiter)
{
    // iterates z = z + c until |z| >= 2 or maxiter is reached,
    // returns the number of iterations.
    complex z = c;
    int n=0;
    for(; n<maxiter; ++n)
    {
        if( std::abs(z) >= 2.0) break;
        z = z*z + c;
    }
    return n;
}

int main(int argc, char **argv)
{
    const int thread_cnt = atoi(argv[1]);
    const int width = 78, height = 44, num_pixels = width*height;
    int mandel_vals[height][width];

    const complex center(-.7, 0), span(2.7, -(4/3.0)*2.7*height/width);
    const complex begin = center-span/2.0;//, end = center+span/2.0;
    const int maxiter = 100000;

    double start, finish;

    start = omp_get_wtime();

    int schd_boundary = num_pixels/2;

    #pragma omp parallel num_threads(thread_cnt)
    {
        #pragma omp for schedule(static) nowait
        for(int pix=0; pix<schd_boundary; ++pix)
        {
            const int x = pix%width, y = pix/width;

            complex c = begin + complex(x * span.real() / (width +1.0),
                                        y * span.imag() / (height+1.0));

            int n = MandelbrotCalculate(c, maxiter);
            if(n == maxiter) n = 0;

            mandel_vals[y][x] = n;
        }

        #pragma omp for schedule(dynamic)
        for(int pix=schd_boundary; pix<num_pixels; ++pix)
        {
            const int x = pix%width, y = pix/width;

            complex c = begin + complex(x * span.real() / (width +1.0),
                                        y * span.imag() / (height+1.0));

            int n = MandelbrotCalculate(c, maxiter);
            if(n == maxiter) n = 0;

            mandel_vals[y][x] = n;
        }

        /*
        int tid = omp_get_thread_num();
        int tnum = omp_get_num_threads();

        int start = tid * num_pixels / tnum;
        int end = (tid+1) * num_pixels / tnum;

        for (int pix = start; pix < end; ++pix) {
            const int x = pix % width, y = pix / width;

            complex c = begin + complex(x * span.real() / (width + 1.0),
                                        y * span.imag() / (height + 1.0));

            int n = MandelbrotCalculate(c, maxiter);
            if (n == maxiter) n = 0;

            mandel_vals[y][x] = n;
        }
        */
    }

    finish = omp_get_wtime();

    /*
    for (int y=0; y<height; ++y) {
        for (int x=0; x<width; ++x) {
            int n = mandel_vals[y][x];
            char c = ' ';
            if (n > 0) {
                static const char charset[] = ".,c8M@jawrpogOQEPGJ";
                c = charset[n % (sizeof(charset) - 1)];
            }
            std::putchar(c);
            if (x + 1 == width) std::puts("|");
        }
    }
    */

    printf("Elapsed Time : %.4f(s)\n", finish - start);
}