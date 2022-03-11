#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "stack.h"


void print_tree_part(char *filename, int depth)
{
    //
    for (int i = 0; i < depth; ++i )
        printf(i != depth - 1  ? "    "   : "└───");

    printf("%s\n", filename);
}


int dopath(char *filename, int *depth, node_t **stack)
{
    if ((strcmp(filename, "..") == 0) && *depth)
    {
        (*depth)--;
        chdir(filename);
        return 0;
    }

    struct stat statBuf;

    if (lstat(filename, &statBuf) == -1)
    {
        printf("Ошибка при получении информации о файле %s!\n", filename);
        return -1;
    }

    if (!S_ISDIR(statBuf.st_mode))
    {
        print_tree_part(filename, *depth);
        return 0;
    }

    print_tree_part(filename, *depth);

    DIR *dp;

    if ((dp = opendir(filename)) == NULL)
    {
        printf("Ошибка при открытии каталога %s!\n", filename);
        return -1;
    }

    if (chdir(filename) == -1)
    {
        closedir(dp);
        puts("Ошибка при изменении текущего рабочего каталога!");
        return -1;
    }

    (*depth)++;
    struct dirent *dirp;

    push(stack, "..");

    while ((dirp = readdir(dp)) != NULL)
    {
        if ((strcmp(dirp->d_name, ".") != 0)
                && (strcmp(dirp->d_name, "..") != 0))
            push(stack, dirp->d_name);
    }

    if (closedir(dp) == -1)
    {
        printf("Ошибка при закрытии каталога %s!\n", filename);
        return -1;
    }

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

    int error = 0;
    int depth = 0;
    node_t *stack = NULL;
    push(&stack, pathname);

    while (!is_empty(&stack) && !error)
    {
        pop(&stack, &buf);
        error = dopath(buf, &depth, &stack);
    }
    
    return 0;
}
