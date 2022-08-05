#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


#define SERV_PORT 9999

void sys_err(const char *str)
{
		perror(str);
		exit(1);
}


int main(int argc, char *argv[])
{
        int cfd;
        struct sockaddr_in serv_addr;          //服务器地址结构
        int counter = 10; 
        char buf[BUFSIZ];

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(SERV_PORT);

        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

        cfd = socket(AF_INET, SOCK_STREAM, 0); 
        if(cfd == -1){
                sys_err("socket error!");
        }   

        int ret = connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if(ret != 0){ 
                sys_err("connect error!");
        }   

        while(1){
                fgets(buf, sizeof(buf), stdin); //从标准输入中读取数
                write(cfd, buf, strlen(buf)); //写到服务器中c_fd既有接收端又有发送端。
                ret = read(cfd, buf, sizeof(buf)); //读从服务器返回的数据
                write(STDOUT_FILENO, buf, ret); //写到屏幕
        }   


        close(cfd);


        return 0;
}

