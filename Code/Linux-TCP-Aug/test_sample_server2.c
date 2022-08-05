#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdbool.h>

#define BUFF_SIZE 30

void error_handling(char *message);
void read_child_proc(int sig);

int main(int argc , char *argv[]){

    //服务端和客户端socket
    int server_socket;
    int client_socket;
    //服务端和客户端地址
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    //用于保存进程ID
    pid_t pid;
    //信号量结构体变量
    struct sigaction act;
    //用于保存socket地址长度
    socklen_t addr_size;
    //用于保存字符串长度
    int str_len;
    //用于记录设置信号量的结果
    int state;
    //字符缓冲
    char buff[BUFF_SIZE];
    //用于控制程序的结束与否
    bool is_running = true;
    //检查传入的参数个数是否合法
    if(argc!=2){
        printf("Usage : %s <port> \n",argv[0]);
        exit(1);
    }

    //初始化信号量机制
    act.sa_handler = read_child_proc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    state = sigaction(SIGCHLD,&act,0);

    //初始化socket
    server_socket = socket(PF_INET,SOCK_STREAM,0);
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    //绑定地址
      if(bind(server_socket,(struct sockaddr *) &server_addr,sizeof(server_addr)) == -1){
        error_handling("bind() error");
      }
    //设置监听
    if(listen(server_socket,5) == -1){
        error_handling("listen() error");
    }
    //通过循环来不断的提供服务
    while(is_running){
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket,(struct sockaddr *) &client_addr,&addr_size);
        //进行检查
        if(client_socket == -1){
            printf("accept error\n");
            continue;
        }else{
            printf("new client connected...............\n");
        }
        //创建新的进程
        pid = fork();
        //检查是否创建成功
        if(pid == -1){
            close(client_socket);
            continue;
        }
        //子进程运行部分，在这里进行服务端和客户端的交互
        if(pid == 0){
            close(server_socket);
            //不断的向客户端发送读取到的数据，直到读取完毕
            while((str_len = read(client_socket,buff,BUFF_SIZE)) != 0){
                write(client_socket,buff,str_len);
            }
            //发送完毕之后关闭客户端的连接
            close(client_socket);
            puts("client disconnected.........");
            //子进程完成任务，返回
            return 0;
        }else{
            close(client_socket);
        }
    }
    //彻底关闭服务端，但是由于前边的while循环是死循环，正常情况下执行不到
    close(server_socket);
return 0;
}
/**子进程处理函数**/
void read_child_proc(int sig){
    pid_t  pid;
    int status;
    //在信号量处理函数中调用waitpid()函数来读取子进程的结束信息，彻底销毁子进程，同时父进程也可以根据status中的信息来对子进程的处理结果进程进一步的处理
    pid = waitpid(-1,&status,WNOHANG);
    printf("remove proc id : %d \n",pid);
}
/**出错处理函数**/
void error_handling(char * message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}