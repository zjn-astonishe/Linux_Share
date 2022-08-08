#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

void error_handle(char *message);
void *read_thread();
void *write_thread();

static pthread_mutex_t is_connect_lock, is_close_lock, printf_lock;
static pthread_cond_t cond;

bool is_connect, is_close;
int client_socket;

int main(int argc, char *argv[])
{

    struct sockaddr_in server_addr;

    if (argc != 3)
    {
        error_handle("Need two param, <IP> <port>");
    }

    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        error_handle("Client Socket Error");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    pthread_mutex_init(&is_connect_lock, NULL);
    pthread_mutex_init(&is_close_lock, NULL);
    pthread_mutex_init(&printf_lock, NULL);
    pthread_cond_init(&cond, NULL);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handle("Connect Error");
    }
    else
    {
        pthread_mutex_lock(&is_connect_lock);
        is_connect = true;
        pthread_mutex_unlock(&is_connect_lock);
        pthread_mutex_lock(&is_close_lock);
        is_close = false;
        pthread_mutex_unlock(&is_close_lock);
    }

    pthread_t id, id1;
    int res;
    res = pthread_create(&id1, NULL, write_thread, NULL);
    assert(res == 0);
    res = pthread_create(&id, NULL, read_thread, NULL);
    assert(res == 0);

    // char message[128];
    // int str_len = read(client_socket, message, sizeof(message) - 1);
    // if(str_len == -1)
    // {
    //     error_handle("Read Error");
    // }
    // printf("Message From Server: %s\n", message);
    // 发送数据
    while (!is_close)
        ;
    close(client_socket);

    pthread_mutex_destroy(&is_close_lock);
    pthread_mutex_destroy(&is_connect_lock);
    pthread_mutex_destroy(&printf_lock);
    pthread_cond_destroy(&cond);

    return 0;
}

void *write_thread()
{
    char message[128];
    int str_len;

    while (is_connect)
    {
        printf("Input contect: ");
        fgets(message, 128, stdin);
        message[strlen(message) - 1] = '\0';
        // printf("%s\n", message);
        // puts(message);
        write(client_socket, message, sizeof(message));
        if (!strcmp(message, "close"))
        {
            pthread_mutex_lock(&is_connect_lock);
            is_connect = false;
            pthread_mutex_unlock(&is_connect_lock);
        }
        pthread_cond_wait(&cond, &printf_lock);
    }
}

void *read_thread()
{
    char message[128];
    int str_len;
    pthread_mutex_lock(&is_close_lock);
    is_close = false;
    pthread_mutex_unlock(&is_close_lock);
    do
    {        
        if((str_len = read(client_socket, message, 128)) == 0)
            error_handle("Read Message From Server Error\n");
        printf("Message from server: %s\n", message);
        pthread_cond_signal(&cond);
    }while (is_connect);
    // printf("Thread close\n");
    pthread_mutex_lock(&is_close_lock);
    is_close = true;
    pthread_mutex_unlock(&is_close_lock);
}

void error_handle(char *message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}