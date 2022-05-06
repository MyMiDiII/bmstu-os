#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

#define GREEN "\033[01;38;05;46m"
#define BLUE  "\033[01;38;05;33m"
#define CLEAR "\033[0m"

void fileInfo(FILE *fs)
{
    printf("%p\n", fs);
	struct stat statbuf;

	stat("resultths.txt", &statbuf);
	printf("inode: %ld\n", statbuf.st_ino);
	printf("Общий размер в байтах: %ld\n", statbuf.st_size);
	printf("Текущая позиция: %ld\n\n", ftell(fs));

    // ??? Текущая позиция?
}


struct args_struct
{
    char begin;
    char * color;
};


void *write_syms(void *args)
{
    FILE *fs = fopen("resultths.txt", "w");
    fileInfo(fs);

    struct args_struct *cur_args = (struct args_struct *) args;
    char begin = cur_args->begin;
    char *color = cur_args->color;

    for (char ch = begin; ch <= 'z'; ch += 2)
        fprintf(fs, "%s%c" CLEAR, color, ch);

    fileInfo(fs);
    fclose(fs);
    fileInfo(fs);

    return NULL;
}

int main(void)
{
    struct args_struct args1 = { .begin = 'a', .color = GREEN };
    struct args_struct args2 = { .begin = 'b', .color = BLUE };

    pthread_t td;
    pthread_create(&td, NULL, write_syms, &args2);

    write_syms(&args1);

    pthread_join(td, NULL);

    return 0;
}
