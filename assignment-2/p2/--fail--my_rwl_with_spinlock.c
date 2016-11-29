#include "my_rwl.h"

static inline int my_atomic_integer_read(int* p_integer)
{
    return __sync_fetch_and_add(p_integer, 0);
}

static inline int my_atomic_integer_write(int* p_integer, int val)
{
    return __sync_val_compare_and_swap(p_integer, *p_integer, val);
}

static inline int my_atomic_integer_inc(int* p_integer)
{
    return __sync_fetch_and_add(p_integer, 1);
}

static inline int my_atomic_integer_dec(int* p_integer)
{
    return __sync_fetch_and_sub(p_integer, 1);
}

static inline int my_atomic_integer_add(int* p_integer, int val)
{
    return __sync_fetch_and_add(p_integer, val);
}

static inline int my_atomic_integer_sub(int* p_integer, int val)
{
    return __sync_fetch_and_sub(p_integer, val);
}

static inline int my_atomic_integer_cas(int* p_integer, int old_val, int new_val)
{
    return __sync_val_compare_and_swap(p_integer, old_val, new_val);
}

static int enter_critical_section(pthread_mutex_t* mutex)
{
    int spinlock = 600;

    while(spinlock-- > 0) {
        if(pthread_mutex_trylock(mutex) == 0)
            return 0;
    }

    return pthread_mutex_lock(mutex);
}

static inline int leave_critical_section(pthread_mutex_t* mutex)
{
    return pthread_mutex_unlock(mutex);
}

int my_rwlock_init(my_rwlock_t* p_rwlock, const char* __not_used__)
{
    int rv;
    if (pthread_mutex_init(&p_rwlock->mutex, NULL) != 0) goto ERROR_MUTEX;
    if (pthread_cond_init(&p_rwlock->cv_read, NULL) != 0) goto ERROR_CV_READ;
    if (pthread_cond_init(&p_rwlock->cv_write, NULL) != 0) goto ERROR_CV_WRITE;

    p_rwlock->reader_cnt = p_rwlock->writer_cnt = p_rwlock->writing = 0;
    rv = 0;
    goto SUCCESS;

ERROR_CV_WRITE:
    pthread_cond_destroy(&p_rwlock->cv_read);
ERROR_CV_READ:
    pthread_mutex_destroy(&p_rwlock->mutex);
ERROR_MUTEX:
    rv = -1;

SUCCESS:
    return rv;
}

int my_rwlock_destroy(my_rwlock_t* p_rwlock)
{
    int fail = 0;
    fail = fail || pthread_mutex_destroy(&p_rwlock->mutex);
    fail = fail || pthread_cond_destroy(&p_rwlock->cv_read);
    fail = fail || pthread_cond_destroy(&p_rwlock->cv_write);
    if (fail)
        return -1;
    else
        return 0;
}

/*
 * 작동하고있는 writer가 있는 지 확인.
 * 없으면 바로 진입.
 */
int my_rwlock_rdlock(my_rwlock_t* p_rwlock)
{
    enter_critical_section(&p_rwlock->mutex);
    ++p_rwlock->reader_cnt;
    if (p_rwlock->writing)
        pthread_cond_wait(&p_rwlock->cv_read, &p_rwlock->mutex);
    leave_critical_section(&p_rwlock->mutex);

    return 0;
}

int my_rwlock_wrlock(my_rwlock_t* p_rwlock)
{
    enter_critical_section(&p_rwlock->mutex);
    ++p_rwlock->writer_cnt;
    if (p_rwlock->writing || p_rwlock->reader_cnt > 0)
        pthread_cond_wait(&p_rwlock->cv_write, &p_rwlock->mutex);
    p_rwlock->writing = 1;
    leave_critical_section(&p_rwlock->mutex);

    return 0;
}

int my_rwlock_unlock(my_rwlock_t* p_rwlock)
{
    enter_critical_section(&p_rwlock->mutex);

    if (p_rwlock->writing) {
        --p_rwlock->writer_cnt;

        if (p_rwlock->reader_cnt > 0) {   // reader preference
            p_rwlock->writing = 0;
            pthread_cond_broadcast(&p_rwlock->cv_read);
        }
        else if (p_rwlock->writer_cnt > 0) {
            pthread_cond_signal(&p_rwlock->cv_write);
        }
        else {
            p_rwlock->writing = 0;
        }
    }
    else {
        --p_rwlock->reader_cnt;

        if (p_rwlock->reader_cnt > 0) {
            pthread_cond_broadcast(&p_rwlock->cv_read);
        }
        else if (p_rwlock->writer_cnt > 0) {
            p_rwlock->writing = 1;
            pthread_cond_signal(&p_rwlock->cv_write);
        }
    }

    leave_critical_section(&p_rwlock->mutex);

    return 0;
}