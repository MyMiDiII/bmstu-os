#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#define GREEN "\033[01;38;05;46m"
#define BLUE  "\033[01;38;05;33m"
#define CLEAR "\033[0m"

struct args_struct
{
    FILE * fs;
    char * color;
};

void *read_buf(void *args)
{
    struct args_struct *cur_args = (struct args_struct *) args;
    FILE *fs = cur_args->fs;
    char *color = cur_args->color;
    int flag = 1;

    while(flag == 1)
    {
        char c;

        flag = fscanf(fs, "%c", &c);
        if (flag == 1) 
            fprintf(stdout, "%s%c" CLEAR, color, c);
    }

    return NULL;
}

int main(void)
{
    int fd = open("alphabet.txt", O_RDONLY);

    FILE *fs1 = fdopen(fd, "r");
    char buff1[20];
    setvbuf(fs1, buff1, _IOFBF, 20);
    struct args_struct args1 = { .fs = fs1, .color = GREEN };

    FILE *fs2 = fdopen(fd, "r");
    char buff2[20];
    setvbuf(fs2, buff2, _IOFBF, 20);
    struct args_struct args2 = { .fs = fs2, .color = BLUE };

    pthread_t td;
    pthread_create(&td, NULL, read_buf, &args2);

    read_buf(&args1);

    pthread_join(td, NULL);
    fprintf(stdout, "\n");
    return 0;
}
