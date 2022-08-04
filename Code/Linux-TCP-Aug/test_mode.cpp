#include <stdio.h>
union bit
{
    short n;
    char byte[2];
};
int main(int argc, char *argv[])
{
    union bit i;
    i.n = 0x1234;
    if (i.byte[0] == 0x34)
        printf("little\n");
    else
        printf("big\n");
    return 0;
}