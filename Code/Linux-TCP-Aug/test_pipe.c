// pipe用于父子进程间通讯。1.父进程创建爱你pipe，父进程调用fork函数创建子进程
//父进程关闭一端
//子进程关闭一端
//父进程和子进程分别执行read或者write操作
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    //创建管道，fd[0]读取，fd[1]写入
    int fd[2];
    int ret = pipe(fd);
    if (ret < 0)
    {
        perror("pipe error");
        return -1;
    }
    //创建子进程
    pid_t pid = fork();

    if (pid < 0)
    { //说明创建子进程失败
        perror("create fork fail");
        return -1;
    }
    else if (pid > 0)
    { //说明是父进程
        //关闭读端
        close(fd[0]);
        sleep(5);
        write(fd[1], "hello world", strlen("hello world"));
        wait(NULL);
    }
    else
    { //说明是子进程，此处也等价于pid==0,
        //关闭写端
        close(fd[1]);
        //用于接收数据
        char buf[64];
        memset(buf, 0x00, sizeof(buf));
        int n = read(fd[0], buf, sizeof(buf));
        printf("read over,n==[%d],buf==[%s]\n", n, buf);
    }
    return 0;
}