#include "request_queue.h"

int request_queue_init(struct request_queue *q)
{
    q->front = 0;
    q->rear = -1;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cv_notfull, NULL);
    pthread_cond_init(&q->cv_notempty, NULL);
    return 0;
}

int request_queue_destroy(struct request_queue *q)
{
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cv_notfull);
    pthread_cond_destroy(&q->cv_notempty);
    return 0;
}

int request_queue_push(struct request_queue *q, struct request *req)
{
    pthread_mutex_lock(&q->mutex);
    int new_rear = (q->rear+1) % REQUEST_QUEUE_SIZE;
    if (new_rear == q->front) {
        // full
        pthread_cond_wait(&q->cv_notfull, &q->mutex);
        new_rear = (q->rear+1) % REQUEST_QUEUE_SIZE;
    }
    q->reqs[new_rear] = req;
    q->rear = new_rear;
    pthread_cond_signal(&q->cv_notempty);  // must be with in critical section.
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int request_queue_pop(struct request_queue *q, struct request **p_req)
{
    pthread_mutex_lock(&q->mutex);
    if (q->front == (q->rear+1) % REQUEST_QUEUE_SIZE) {
        // empty
        pthread_cond_wait(&q->cv_notempty, &q->mutex);
    }
    *p_req = q->reqs[q->front];
    q->front = (q->front+1) % REQUEST_QUEUE_SIZE;
    pthread_cond_signal(&q->cv_notfull);  // must be with in critical section.
    pthread_mutex_unlock(&q->mutex);
    return 0;
}