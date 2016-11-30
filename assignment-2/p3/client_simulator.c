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


struct simulate_info {
    char *srv_ip;   /* Don't modify deeply in a thread. It is shared. */
    unsigned short srv_port;
    int thread_num;
    int access_num;
    int path_num;
    char **paths;   /* Don't modify deeply in a thread. It is shared. */
};

void* request_thread_main(void *param)
{
    struct simulate_info *p_sm_info = (struct simulate_info *) param;
}

int main(int argc, char **argv)
{
    struct simulate_info org_sm_info;
    struct simulate_info *sm_info_list;
    pthread_t *tids;

    if (argc <= 6) {
        printf("Usage : %s <server ip> <server port> <the number of request threads>"
                       " <the number of accesses per a thread>"
                       " <the number of request paths> <path 1> <path 2> ... \n",
               argv[0]);
        return -1;
    }

    org_sm_info.srv_ip = argv[1];
    org_sm_info.srv_port = atoi(argv[2]);
    org_sm_info.thread_num = atoi(argv[3]);
    org_sm_info.access_num = atoi(argv[4]);
    org_sm_info.path_num = atoi(argv[5]);
    org_sm_info.paths = &argv[6];

    printf("Creating %d reqeust threads!\n", org_sm_info.thread_num);

    sm_info_list = malloc(sizeof(*sm_info_list) * org_sm_info.thread_num);
    tids = malloc(sizeof(*tids) * org_sm_info.thread_num);

    for(int i=0; i<org_sm_info.thread_num; ++i) {
        pthread_attr_t attr;
        sm_info_list[i] = org_sm_info;
        pthread_attr_init(&attr);
        pthread_create(&tids[i], &attr, request_thread_main, &sm_info_list[i]);
    }

    // join all threads.
    for(int i=0; i<org_sm_info.thread_num; ++i) {
        pthread_join(tids[i], NULL);
    }

    free(sm_info_list);
    free(tids);

    return 0;
}