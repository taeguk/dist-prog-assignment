#ifndef _MY_RWL_H
#define _MY_RWL_H

#include <pthread.h>

typedef struct my_rwlock_t {
    pthread_mutex_t mutex;      // mutex for variables.
    pthread_cond_t cv_read;     // for signaling to read thread.
    pthread_cond_t cv_write;    // for signaling to write thread.
    int reader_cnt;
    int writer_cnt;
    int writing;
} my_rwlock_t;


int my_rwlock_init(my_rwlock_t* p_rwlock, const char* __not_used__);
int my_rwlock_destroy(my_rwlock_t* p_rwlock);
int my_rwlock_rdlock(my_rwlock_t* p_rwlock);
int my_rwlock_wrlock(my_rwlock_t* p_rwlock);
int my_rwlock_unlock(my_rwlock_t* p_rwlock);

#endif //DIST_PROG_ASSIGNMENT_MY_RWL_H
