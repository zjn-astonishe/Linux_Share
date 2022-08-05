#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// 网络编程所需头文件
#include <arpa/inet.h>
#include <sys/socket.h>
// 多进程程序所需头文件
#include <sys/wait.h>
#include <signal.h>

bool is_running = true;             // 服务器是否运转
int client_number = 0;              // 连接的客户端数量
void error_handle(char *message);
void deal_child(int signal_state);
void sig_handler(int num);

int main(int argc, char *argv[])
{
    int server_socket, client_socket;   // 服务器和客户端的socket
    struct sockaddr_in server_addr;     // 服务器地址
    struct sockaddr_in client_addr;     // 客户端地址
    socklen_t client_addr_size;         // 客户端地址长度
    
    char message[128];                  // 字符串缓存

    pid_t pid;                          // 进程ID
    struct sigaction signal1;           // 信号量结构体变量


    signal1.sa_handler = deal_child;                 // 信号量处理函数的地址
    signal1.sa_flags = 0;                            // 初始化信号处理标志
    sigemptyset(&signal1.sa_mask);                   // 初始化mask信号，并清空
    sigaction(SIGCHLD, &signal1, 0);         // SIGCHLD是子进程释放的信号量，用来设置(可查询)信号处理方式。

    signal(SIGALRM, sig_handler);           //SIGALRM是在定时器终止时发送给进程的信号
    
    if(argc != 2)
    {
        error_handle("Program need only one param <port>");
    }

    // 创建socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);    // PF_INET和AF_INET都指的是IPV4域，SOCK_STREAM是面向连接的流式
    if(server_socket == -1)
    {
        error_handle("Create socket error!");
    }
    int on = 1;
    int ser_socket = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(ser_socket < 0)
    {
        error_handle("SetSocket Error");
    }

    // 设置地址
    memset(&server_addr, 0, sizeof(server_addr));   // 地址初始化
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);       // 任意IP地址，以网络字节序表示的32位整数
    server_addr.sin_port = htons(atoi(argv[1]));

    // 绑定socket和地址
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handle("Socket and addr bind error!");
    }

    // 监听客户端连接请求
    if(listen(server_socket, 5) == -1)
    {
        error_handle("Listen errir");
    }

    while(is_running)
    {
        if(client_number == 0)
            alarm(10);
        // 接收客户端的连接请求
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        printf("%s\n", message);
        if(client_socket == -1)
        {
            printf("Accept Error\n");
            continue;
        }
        else
        {
            // if(client_number <= 10)
            // {
            printf("new client %d connected............\n", ++client_number);
            // }
            // else
            // {
            //     continue;
            // }
        }
        pid = fork();
        if(pid == -1)   // 创建子进程失败
        {
            close(client_socket);
            printf("Create child process failed\n");
            continue;
        }
        else if(pid == 0)   // 子进程程序
        {
            close(server_socket);
            int str_len = read(client_socket, message, sizeof(message) - 1);
            while(strcmp(message, "close"))
            {
                if(str_len == -1)
                {
                    printf("Read Message Error\n");
                    continue;
                }
                printf("Client Message: %s\n", message);
                write(client_socket, message, str_len);
                str_len = read(client_socket, message, sizeof(message) - 1);
            }
            close(client_socket);
            printf("Client disconnected.............\n");
            return 0;
        }
        else
        {
            close(client_socket);   // 主进程关闭与客户端的连接
        }
    }

    close(server_socket);
    return 0;
    
}

void sig_handler(int num)
{
    printf("alarm handle\n");
    if(client_number == 0)
        is_running = false;
}

void deal_child(int signal_state)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("remove child process id: %d\n", pid);
    client_number--;
}

void error_handle(char* message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}