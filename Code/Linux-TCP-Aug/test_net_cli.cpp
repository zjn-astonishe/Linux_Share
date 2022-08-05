#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

void error_handle(char *message);

int main(int argc, char *argv[])
{
    int client_socket;
    bool is_connect;
    struct sockaddr_in server_addr;
    
    if(argc != 3)
    {
        error_handle("Need two param, <IP> <port>");
    }

    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(client_socket == -1)
    {
        error_handle("Client Socket Error");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handle("Connect Error");
    }
    else
        is_connect = true;

    // char message[128];
    // int str_len = read(client_socket, message, sizeof(message) - 1);
    // if(str_len == -1)
    // {
    //     error_handle("Read Error");
    // }
    // printf("Message From Server: %s\n", message);
    // 发送数据
    string input;
    char message[128];
    while(is_connect)
    {
        cout << "Input content: ";
        cin >> message;
        cout << message << endl;
        write(client_socket, message, sizeof(message));
        if(!strcmp(message, "close"))
            is_connect = false;
    }
    
    
    close(client_socket);
    return 0;
}

void error_handle(char* message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}