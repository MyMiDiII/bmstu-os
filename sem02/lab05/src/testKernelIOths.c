#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#define GREEN "\033[01;38;05;46m"
#define BLUE  "\033[01;38;05;33m"
#define CLEAR "\033[0m"

struct args_struct { int fd; char * color; };

void *read_buf(void *args)
{
    struct args_struct *cur_args = (struct args_struct *) args;
    int fd = cur_args->fd;
    char *color = cur_args->color;

    int flag = 1;

    while (flag == 1)
    {
        char c;
        if ((flag = read(fd, &c, 1)) == 1)
            printf("%s%c" CLEAR, color, c);
    }

    return NULL;
}

int main()
{
    char c;    
    int fd1 = open("alphabet.txt",O_RDONLY);
    struct args_struct args1 = { .fd = fd1, .color = GREEN };

    int fd2 = open("alphabet.txt",O_RDONLY);
    struct args_struct args2 = { .fd = fd2, .color = BLUE };

    pthread_t td;
    pthread_create(&td, NULL, read_buf, &args2);

    read_buf(&args1);

    pthread_join(td, NULL);
    puts("");
    return 0;
}

