#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Need only one param");
        return 0;
    }
    else
    {
        printf("%s\n", argv[1]);
    }
}