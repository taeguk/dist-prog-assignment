//
// Created by taeguk on 2016-11-25.
//

#ifndef DIST_PROG_ASSIGNMENT_MY_RWL_H
#define DIST_PROG_ASSIGNMENT_MY_RWL_H

#include <pthread.h>

typedef union my_rwlock_t_atomic_vars {
    struct __attribute__((__packed__)) {
#define LSB_OFFSET_READER_CNT sizeof(short)
        short reader_cnt;
#define LSB_OFFSET_WRITER_CNT 0
        short writer_cnt;
    } vars;
    int combined;   // for atomic instructions.
} my_rwlock_t_atomic_vars;

typedef struct my_rwlock_t {
    pthread_mutex_t mutex;      // mutex for cv_read.
    pthread_cond_t cv_read;     // for signaling to read thread.
    pthread_cond_t cv_write;    // for signaling to write thread.
    my_rwlock_t_atomic_vars atomic_vars;
    int reading_cnt;
} my_rwlock_t;


int my_rwlock_init(my_rwlock_t* p_rwlock, const char* __not_used__);
int my_rwlock_destroy(my_rwlock_t* p_rwlock);
int my_rwlock_rdlock(my_rwlock_t* p_rwlock);
int my_rwlock_wrlock(my_rwlock_t* p_rwlock);
int my_rwlock_unlock(my_rwlock_t* p_rwlock);

#endif //DIST_PROG_ASSIGNMENT_MY_RWL_H
