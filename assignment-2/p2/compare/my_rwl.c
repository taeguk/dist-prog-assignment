//
// Created by taeguk on 2016-11-25.
//

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

int my_rwlock_init(my_rwlock_t* p_rwlock, const char* __not_used__)
{
    if (pthread_mutex_init(&p_rwlock->mutex, NULL) != 0) goto ERROR_MUTEX;
    if (pthread_cond_init(&p_rwlock->cv_read, NULL) != 0) goto ERROR_CV_READ;
    if (pthread_cond_init(&p_rwlock->cv_write, NULL) != 0) goto ERROR_CV_WRITE;

    p_rwlock->atomic_vars.vars.reader_cnt = 0;
    p_rwlock->atomic_vars.vars.can_read = 1;
    p_rwlock->reading_cnt = 0;

    return 0;

ERROR_CV_WRITE:
    pthread_cond_destroy(&p_rwlock->cv_read);
ERROR_CV_READ:
    pthread_mutex_destroy(&p_rwlock->mutex);
ERROR_MUTEX:

    return -1;
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
    // increment reader_cnt and fetch writer_.
    int old = my_atomic_integer_add(&p_rwlock->atomic_vars.combined, 1 << LSB_OFFSET_READER_CNT);
    if( ! ((my_rwlock_t_atomic_vars) old).vars.can_read) {
        // when writer thread is having a lock.
        pthread_mutex_lock(&p_rwlock->mutex);
        pthread_cond_wait(&p_rwlock->cv_read, &p_rwlock->mutex);
        pthread_mutex_unlock(&p_rwlock->mutex);
    }
    my_atomic_integer_inc(&p_rwlock->reading_cnt);
    return 0;
}

int my_rwlock_wrlock(my_rwlock_t* p_rwlock)
{
    // set can_read = 0 and fetch reader_cnt.
    int old = __sync_fetch_and_and(&p_rwlock->atomic_vars.combined, 0xFFFF0000);
    if(((my_rwlock_t_atomic_vars) old).vars.reader_cnt > 0) {
        pthread_mutex_lock(&p_rwlock->mutex);
        pthread_cond_wait(&p_rwlock->cv_write, &p_rwlock->mutex);
        pthread_mutex_unlock(&p_rwlock->mutex);
    }
    return 0;
}

int my_rwlock_unlock(my_rwlock_t* p_rwlock)
{
    if(my_atomic_integer_read(&p_rwlock->reading_cnt) > 0) {
        // case of read lock.
        my_atomic_integer_dec(&p_rwlock->reading_cnt);
        int old = my_atomic_integer_sub(&p_rwlock->atomic_vars.combined, 1 << LSB_OFFSET_READER_CNT);
        if(((my_rwlock_t_atomic_vars) old).vars.reader_cnt == 1) {
            pthread_cond_signal(&p_rwlock->cv_write);
        }
    }
    else {
        // case of write lock.
        int val = my_atomic_integer_read(&p_rwlock->atomic_vars.combined);
        if (((my_rwlock_t_atomic_vars) val).vars.reader_cnt == 0) {
            pthread_cond_signal(&p_rwlock->cv_write);
        }
        else {
            pthread_cond_broadcast(&p_rwlock->cv_read);
        }
        // set can_read = 1
        __sync_fetch_and_or(&p_rwlock->atomic_vars.combined, 0x00000001);
    }
    return 0;
}