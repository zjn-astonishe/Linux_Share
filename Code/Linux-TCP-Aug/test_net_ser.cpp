#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handle(char *message);

int main(int argc, char *argv[])
{
    int server_socket, client_socket;   // 服务器和客户端的socket
    struct sockaddr_in server_addr;     // 服务器地址
    struct sockaddr_in client_addr;     // 客户端地址

    socklen_t client_addr_size;     // 客户端地址长度

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

    // 接收客户端的连接请求
    client_addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    if(client_socket == -1)
    {
        error_handle("Accept Error");
    }

    // 发送数据
    char message[] = "Test_Net_Server: Hello!";
    write(client_socket, message, sizeof(message));
    
    // 关闭socket
    close(client_socket);
    close(server_socket);
    return 0;
    
}

void error_handle(char* message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}