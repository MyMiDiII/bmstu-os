#include <stdio.h>
#include <fcntl.h>

#define GREEN "\033[01;38;05;46m"
#define BLUE  "\033[01;38;05;33m"
#define CLEAR "\033[0m"

int main(void)
{
    int fd = open("alphabet.txt", O_RDONLY);

    FILE *fs1 = fdopen(fd, "r");
    char buff1[20];
    setvbuf(fs1, buff1, _IOFBF, 20);

    FILE *fs2 = fdopen(fd, "r");
    char buff2[20];
    setvbuf(fs2, buff2, _IOFBF, 20);

    int flag1 = 1, flag2 = 1;

    while (flag1 == 1 || flag2 == 1)
    {
        char c;

        if ((flag1 = fscanf(fs1, "%c", &c)) == 1)
            fprintf(stdout, GREEN "%c" CLEAR, c);

        if ((flag2 = fscanf(fs2, "%c", &c)) == 1) 
            fprintf(stdout, BLUE "%c" CLEAR, c); 
    }

    fprintf(stdout, "\n");
    return 0;
}
