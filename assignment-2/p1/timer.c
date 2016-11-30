#include "timer.h"
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

static int tod = 0;

static inline void timer_atomic_inc_tod(void)
{
    __sync_fetch_and_add(&tod, 1);
}

inline int timer_atomic_get_tod(void)
{
    return __sync_fetch_and_add(&tod, 0);
}

static void timer_handler (int signum)
{
    printf("timer_handler()!\n");
    timer_atomic_inc_tod();
}

static void* timer_thread_main(void* param)
{
    printf("timer_thread_main()!\n");
    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGVTALRM. */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction (SIGVTALRM, &sa, NULL);

    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    /* Start a virtual timer. It counts down whenever this process is executing. */
    setitimer (ITIMER_VIRTUAL, &timer, NULL);

    while(1);

    pthread_exit(0);
}

int timer_start()
{
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, timer_thread_main, NULL);

    return 0;
}

static void timer_junk_handler (int signum)
{
    printf("timer_junk_handler()!\n");
}

int timer_delay(int interval)
{
    printf("timer_delay(%d)!\n", interval);

    int start_tod = timer_atomic_get_tod();
    int cur_tod = start_tod;

    while(cur_tod - start_tod < interval)
        cur_tod = timer_atomic_get_tod();

    printf("Finish of timer_delay(%d)!\n", interval);

    return cur_tod;
}