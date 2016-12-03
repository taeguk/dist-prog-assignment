#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "http_lib.h"
#include "request_queue.h"

void* worker_thread_main(void* param)
{
    struct request_queue *q = (struct request_queue*) param;

    while(1)
    {
        struct request *req;
        request_queue_pop(q, &req);
        printf("Before process! (%s)\n", req->req_msg);
        process_request(req->req_msg, req->sock);
        printf("After process! (%s)\n", req->req_msg);
        close(req->sock);
        free(req);
    }
}

int main(int argc, char **argv)
{
    int listen_sock, clnt_sock;
    int state, clnt_len;
    int pid;
    unsigned short port;
    int worker_num;

    struct sockaddr_in clnt_addr, srv_addr;

    if (argc != 3) {
        printf("Usage : %s <port> <the number of worker threads>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[1]);
    worker_num = atoi(argv[2]);

    clnt_len = sizeof(clnt_addr);

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error : ");
        return -1;
    }
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(port);

    state = bind(listen_sock , (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (state == -1) {
        perror("bind error : ");
        return -1;
    }

    state = listen(listen_sock, 5);
    if (state == -1) {
        perror("listen error : ");
        return -1;
    }

    struct request_queue rqueue;

    request_queue_init(&rqueue);

    printf("Creating %d worker threads!\n", worker_num);
    for(int i=0; i<worker_num; ++i) {
        pthread_t tid;
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, worker_thread_main, &rqueue);
    }

    while(1)
    {
        clnt_sock = accept(listen_sock, (struct sockaddr *)&clnt_addr,
                               &clnt_len);
        if (clnt_sock == -1) {
            perror("accept error : ");
            continue;
        }

        struct request *req = malloc(sizeof(struct request));
        int cn;

        if ((cn=read(clnt_sock, req->req_msg, MAX_REQUEST_LEN)) == -1) {
            perror("read error : ");
            close(clnt_sock);
            free(req);
            continue;
        }
        req->req_msg[cn] = '\0';

        req->sock = clnt_sock;
        request_queue_push(&rqueue, req);
    }

    request_queue_destroy(&rqueue);

    return 0;
}