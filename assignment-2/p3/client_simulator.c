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
#include <time.h>
#include "timer.h"

#define BUF_SIZE    1024

struct simulate_info {
    int sm_id;
    char *srv_ip;   /* Don't modify deeply in a thread. It is shared. */
    unsigned short srv_port;
    int access_num;
    int path_num;
    char **paths;   /* Don't modify deeply in a thread. It is shared. */
    unsigned int rand_seed;
};

void* request_thread_main(void *param)
{
    struct simulate_info *p_sm_info = (struct simulate_info *) param;
    char request[BUF_SIZE];
    char buf[BUF_SIZE];
    int cn;

    for (int i=0; i<p_sm_info->access_num; ++i) {

        int sock;
        struct sockaddr_in srv_addr;

        if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket error : ");
            --i;
            break;
        }

        memset(&srv_addr, 0, sizeof(srv_addr));

        srv_addr.sin_family = AF_INET;
        srv_addr.sin_addr.s_addr = inet_addr(p_sm_info->srv_ip);
        srv_addr.sin_port = htons(p_sm_info->srv_port);

        if(connect(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
            perror("connect error : ");
            --i;
            close(sock);
            break;
        }

        int rand_num = rand_r(&p_sm_info->rand_seed) % p_sm_info->path_num;

        cn = snprintf(request, BUF_SIZE, "GET %s HTTP/1.0\r\n", p_sm_info->paths[rand_num]);
        request[cn] = 0;

        printf("[Thread %d][#%d] Send GET %s HTTP/1.0\n", p_sm_info->sm_id, i, p_sm_info->paths[rand_num]);

        if(write(sock, request, cn) == -1) {
            perror("write error : ");
            close(sock);
            break;
        }

        int read_bytes = 0;
        cn = read(sock, buf, BUF_SIZE);

        //printf("[Thread %d][#%d] First Read Yeah!\n", p_sm_info->sm_id, i);

        int status = -1;
        if(sscanf(buf, "%*s %d %*s\r\n", &status) != 1) {
            printf("parsing error : %s\n", buf);
            continue;
        }

        while(cn > 0) {
            read_bytes += cn;
            cn = read(sock, buf, BUF_SIZE);
        }

        /*
        if (cn == -1) {
            perror("read error : ");
            close(sock);
            close(sock);
            break;
        }
        */

        if (status == 200) {
            printf("[Thread %d][#%d] Receive %d Bytes. (status : %d)\n", p_sm_info->sm_id, i, read_bytes, status);
        }
        else {
            printf("[Thread %d][#%d] Error (status : %d)\n", p_sm_info->sm_id, i, status);
        }

        close(sock);

        //sleep(1);
    }

    pthread_exit(0);
}

int main(int argc, char **argv)
{
    int thread_num;
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
    thread_num = atoi(argv[3]);
    org_sm_info.access_num = atoi(argv[4]);
    org_sm_info.path_num = atoi(argv[5]);
    org_sm_info.paths = &argv[6];

    printf("Creating %d reqeust threads!\n", thread_num);

    sm_info_list = malloc(sizeof(*sm_info_list) * thread_num);
    tids = malloc(sizeof(*tids) * thread_num);

    for(int i=0; i<thread_num; ++i) {
        sm_info_list[i] = org_sm_info;
        sm_info_list[i].sm_id = i;
        sm_info_list[i].rand_seed = time(NULL) + i * 0xabcdef;
    }

    double start, finish;

    GET_TIME(start);

    // create all threads.
    for(int i=0; i<thread_num; ++i) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[i], &attr, request_thread_main, &sm_info_list[i]);
    }

    // join all threads.
    for(int i=0; i<thread_num; ++i) {
        pthread_join(tids[i], NULL);
    }

    GET_TIME(finish);

    printf("Elapsed Time : %e seconds.\n", finish - start);

    free(sm_info_list);
    free(tids);

    return 0;
}