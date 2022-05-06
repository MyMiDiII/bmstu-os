#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define GREEN "\033[01;38;05;46m"
#define BLUE  "\033[01;38;05;33m"
#define CLEAR "\033[0m"

int main()
{
    char c;    
    int fd1 = open("alphabet.txt",O_RDONLY);
    int fd2 = open("alphabet.txt",O_RDONLY);
    int flag1 = 1, flag2 = 1;

    while(flag1 == 1 || flag2 == 1)
    {
        if ((flag1 = read(fd1, &c, 1)) == 1)
            printf(GREEN "%c" CLEAR, c);

        if ((flag2 = read(fd2, &c, 1)) == 1)
            printf(BLUE "%c" CLEAR, c); 
    }

    puts("");
    return 0;
}

