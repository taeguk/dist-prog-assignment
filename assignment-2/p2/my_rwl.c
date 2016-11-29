#include "my_rwl.h"

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
    if (p_rwlock->writing)
        pthread_cond_wait(&p_rwlock->cv_read, &p_rwlock->mutex);
    pthread_mutex_unlock(&p_rwlock->mutex);

    return 0;
}

int my_rwlock_wrlock(my_rwlock_t* p_rwlock)
{
    pthread_mutex_lock(&p_rwlock->mutex);
    ++p_rwlock->writer_cnt;
    if (p_rwlock->writing || p_rwlock->reader_cnt > 0)
        pthread_cond_wait(&p_rwlock->cv_write, &p_rwlock->mutex);
    p_rwlock->writing = 1;
    pthread_mutex_unlock(&p_rwlock->mutex);

    return 0;
}

/*
 * a little faster than --simple-- version.
 */
int my_rwlock_unlock(my_rwlock_t* p_rwlock)
{
    pthread_mutex_lock(&p_rwlock->mutex);

    if (p_rwlock->writing) {
        --p_rwlock->writer_cnt;

        if (p_rwlock->reader_cnt > 0) {   // reader preference
            p_rwlock->writing = 0;
            pthread_mutex_unlock(&p_rwlock->mutex);
            pthread_cond_broadcast(&p_rwlock->cv_read);
        }
        else if (p_rwlock->writer_cnt > 0) {
            pthread_mutex_unlock(&p_rwlock->mutex);
            pthread_cond_signal(&p_rwlock->cv_write);
        }
        else {
            p_rwlock->writing = 0;
            pthread_mutex_unlock(&p_rwlock->mutex);
        }
    }
    else {
        --p_rwlock->reader_cnt;

        if (p_rwlock->reader_cnt > 0) {
            pthread_mutex_unlock(&p_rwlock->mutex);
            pthread_cond_broadcast(&p_rwlock->cv_read);
        }
        else if (p_rwlock->writer_cnt > 0) {
            p_rwlock->writing = 1;
            pthread_mutex_unlock(&p_rwlock->mutex);
            pthread_cond_signal(&p_rwlock->cv_write);
        }
        else {
            pthread_mutex_unlock(&p_rwlock->mutex);
        }
    }

    return 0;
}