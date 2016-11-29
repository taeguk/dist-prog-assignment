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

    p_rwlock->reader_cnt = p_rwlock->writer_cnt = p_rwlock->writing = 0;

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
    pthread_mutex_lock(&p_rwlock->mutex);
    ++p_rwlock->reader_cnt;
    // printf("my_rwlock_rdlock! %d %d %d\n", p_rwlock->reader_cnt, p_rwlock->writer_cnt, p_rwlock->writing);
    if (p_rwlock->writing)
        pthread_cond_wait(&p_rwlock->cv_read, &p_rwlock->mutex);
    // printf("Reader ger a lock!\n");
    pthread_mutex_unlock(&p_rwlock->mutex);

    return 0;
}

int my_rwlock_wrlock(my_rwlock_t* p_rwlock)
{
    pthread_mutex_lock(&p_rwlock->mutex);
    ++p_rwlock->writer_cnt;
    // printf("my_rwlock_wrlock! %d %d %d\n", p_rwlock->reader_cnt, p_rwlock->writer_cnt, p_rwlock->writing);
    if (p_rwlock->writing || p_rwlock->reader_cnt > 0)
        pthread_cond_wait(&p_rwlock->cv_write, &p_rwlock->mutex);
    // printf("Writer get a lock!\n");
    p_rwlock->writing = 1;
    pthread_mutex_unlock(&p_rwlock->mutex);

    return 0;
}

int my_rwlock_unlock(my_rwlock_t* p_rwlock)
{
    pthread_mutex_lock(&p_rwlock->mutex);

    if (p_rwlock->writing) {

        --p_rwlock->writer_cnt;

        if (p_rwlock->reader_cnt > 0) {   // reader preference
            p_rwlock->writing = 0;
            // printf("my_rwlock_unlock! (writer) %d %d %d\n", p_rwlock->reader_cnt, p_rwlock->writer_cnt, p_rwlock->writing);
            pthread_cond_broadcast(&p_rwlock->cv_read);
        }
        else if (p_rwlock->writer_cnt > 0) {
            // printf("my_rwlock_unlock! (writer) %d %d %d\n", p_rwlock->reader_cnt, p_rwlock->writer_cnt,p_rwlock->writing);
            pthread_cond_signal(&p_rwlock->cv_write);
        }
        else {
            p_rwlock->writing = 0;
            // printf("my_rwlock_unlock! (writer) %d %d %d\n", p_rwlock->reader_cnt, p_rwlock->writer_cnt, p_rwlock->writing);
        }
    }
    else {

        --p_rwlock->reader_cnt;
        // printf("my_rwlock_unlock! (reader) %d %d %d\n", p_rwlock->reader_cnt, p_rwlock->writer_cnt, p_rwlock->writing);

        if (p_rwlock->reader_cnt > 0)
            pthread_cond_broadcast(&p_rwlock->cv_read);
        else if (p_rwlock->writer_cnt > 0) {
            // printf("pthread_cond_signal(&p_rwlock->cv_write)\n");
            p_rwlock->writing = 1;
            pthread_cond_signal(&p_rwlock->cv_write);
        }
    }

    pthread_mutex_unlock(&p_rwlock->mutex);

    return 0;
}