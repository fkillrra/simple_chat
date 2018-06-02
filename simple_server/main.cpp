#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 100

void *handle_clnt(void *arg);   //thread main() function.
void send_msg(char *msg, int len);  //Send a message to all clients.
void error(char *msg);  //print error message.

int clnt_cnt = 0;   //Number of clients.
int all_clnt[MAX_CLNT];   //client descripter.
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    if(argc != 2)
    {
        printf("Usage : %s [port]\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);    //create mutex
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error("bind() error");

    if(listen(serv_sock, 5) == -1)
        error("listen() error");

    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        if(clnt_sock == -1)
            error("accept() error");

        pthread_mutex_lock(&mutx);
        all_clnt[clnt_cnt++] = clnt_sock; //Critical section.
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);   //Completely removed from memory
        printf("Connected client\n");
    }
    close(serv_sock);
    pthread_mutex_destroy(&mutx);   //destroy mutex
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int*)arg);   //descripter
    int str_len = 0;
    char msg[BUF_SIZE];

    while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    pthread_mutex_lock(&mutx);
    for(int i = 0; i < clnt_cnt; i++)   //remove client
    {
        if(clnt_sock == all_clnt[i])
        {
            for(i = i; i < clnt_cnt - 1; i++)
                all_clnt[i] = all_clnt[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char *msg, int len)
{
    pthread_mutex_lock(&mutx);
    for(int i = 0; i < clnt_cnt; i++)   //Send a message to all clients.
        write(all_clnt[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
