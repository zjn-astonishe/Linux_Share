#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
void sig_handler(int num)
{
    printf("receive the signal %d.\n", num);
}
int main()
{
    signal(SIGALRM, sig_handler); //SIGALRM是在定时器终止时发送给进程的信号
    
    alarm(2);
     pause();//pause()函数使该进程暂停让出CPU
    exit(0);
}
