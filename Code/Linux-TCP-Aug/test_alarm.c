#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
int i;
/*闹钟信号处理函数*/
void sig_handler()
{
    if(i % 2 == 0)
        printf("hello world! \n ");
    else 
        printf("dlrow olleh\n");
}

/*主函数*/
int main()
{
    signal(SIGALRM, sig_handler);
    srand((unsigned)time(NULL));
    int time1 = rand()%10;
    printf("time : %d\n", time1);
    alarm(time1);
    for (i = 0; i < 8; i++)
    {
        printf(" sleep % d ... \n ", i);
        sleep(1);
    }
    return 0;
}