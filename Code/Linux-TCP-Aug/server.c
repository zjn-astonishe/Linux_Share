#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define BUFF_SIZE 128

void error_handling(char *message);
void read_child_proc(int sig);
void sig_handler();

int client_num = 0;
int server_socket; // 服务端socket

int main(int argc, char *argv[])
{

    //客户端socket
    int client_socket;
    //服务端和客户端地址
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    //用于保存进程ID
    pid_t pid;
    //信号结构体变量
    struct sigaction act;
    //用于保存socket地址长度
    socklen_t addr_size;
    //用于保存字符串长度
    int str_len;
    //用于记录设置信号的结果
    int state;
    //字符缓冲
    char buff[BUFF_SIZE];
    
    //检查传入的参数个数是否合法
    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    //初始化信号机制
    act.sa_handler = read_child_proc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    state = sigaction(SIGCHLD, &act, 0);

    //初始化socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    signal(SIGALRM, sig_handler);

    //绑定地址
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("bind() error");
    }
    //设置监听
    if (listen(server_socket, 5) == -1)
    {
        error_handling("listen() error");
    }
    printf("Server opened successfully.\n");
    //通过循环来不断的提供服务
    while (1)
    {
        addr_size = sizeof(client_addr);
        alarm(10);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        //进行检查
        if (client_socket == -1)
        {
            continue;
        }
        else
        {
            client_num++;
            //创建新的进程
            
        }
        pid = fork();
            
        //检查是否创建成功，此处为如果创建失败
        if (pid == -1)
        {
            printf("Client Process create error\n");
            close(client_socket);
            continue;
        }
        //子进程运行部分，在这里进行服务端和客户端的交互
        if (pid == 0)
        {
            close(server_socket); // 关闭负责监听的socket，子进程只负责和客户端进行交互。
            //不断的向客户端发送读取到的数据，直到读取完毕
            while ((str_len = read(client_socket, buff, BUFF_SIZE)) != 0)
            {
                printf("Message received from client %d: ", getpid());
                puts(buff);
                write(client_socket, buff, str_len);
                memset(&buff, 0, sizeof(buff));
            }
            //发送完毕之后关闭客户端交互socket
            close(client_socket);
            puts("client disconnected.........");
            //子进程完成任务，返回
            exit(0);
        }
        else
        {
            printf("new client %d connected...............\n", pid);
            printf("Client num: %d\n", client_num);
            close(client_socket); // 主进程只负责监听连接，不负责和客户端进行交互，所以关闭。
        }
        // printf("client num: %d\n", client_num);
    }
    //彻底关闭服务端，但是由于前边的while循环是死循环，正常情况下执行不到
    close(server_socket);
    return 0;
}
/**子进程处理函数**/
void read_child_proc(int sig)
{
    pid_t pid;
    int status;
    //在信号处理函数中调用waitpid()函数来读取子进程的结束信息，彻底销毁子进程，同时父进程也可以根据status中的信息来对子进程的处理结果进程进一步的处理
    pid = waitpid(-1, &status, WNOHANG);
    client_num--;
    printf("remove proc id : %d \n", pid);
    printf("Client num: %d\n", client_num);
}

/**出错处理函数**/
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void sig_handler()
{
    // printf("Check\n");
    // printf("client num: %d\n", client_num);
    if (!client_num)
    {
        close(server_socket);
        printf("No client connected to the server for a period of time, close Server\n");
        exit(0);
    }
    else
    {
        alarm(10);
    }
}