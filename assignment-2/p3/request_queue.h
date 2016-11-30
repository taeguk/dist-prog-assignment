#ifndef DIST_PROG_ASSIGNMENT_REQUEST_QUEUE_H
#define DIST_PROG_ASSIGNMENT_REQUEST_QUEUE_H

#include <pthread.h>

#define MAX_REQUEST_LEN 300

struct request {
    char req_msg[MAX_REQUEST_LEN+1];
    int sock;
};

#define REQUEST_QUEUE_SIZE  1000

struct request_queue {
    struct request *reqs[REQUEST_QUEUE_SIZE];
    int front, rear;
    pthread_mutex_t mutex;
    pthread_cond_t cv_notfull, cv_notempty;
};

int request_queue_init(struct request_queue *q);
int request_queue_destroy(struct request_queue *q);
int request_queue_push(struct request_queue *q, struct request *req);
int request_queue_pop(struct request_queue *q, struct request **p_req);

#endif //DIST_PROG_ASSIGNMENT_REQUEST_QUEUE_H
