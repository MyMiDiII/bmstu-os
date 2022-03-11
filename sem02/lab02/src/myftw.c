#include <stdio.h>
#include <unistd.h>

#include "stack.h"

int dopath(char *filename)
{
    printf("%s\n", filename);
    return 0;
}


int myftw(char *pathname)
{
    if (chdir(pathname) == -1)
    {
        puts("Ошибка при изменении текущего рабочего каталога!");

        return -1;
    }

    char name[MAX_FILE_NAME];
    char *buf= name;

    node_t *stack = NULL;
    push(&stack, pathname);

    while (!is_empty(&stack))
    {
        pop(&stack, &buf);
        dopath(buf);
    }
    
    return 0;
}
