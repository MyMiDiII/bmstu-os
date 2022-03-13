#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <errno.h>

#include "stack.h"


void print_tree_part(char *filename, int depth)
{
    for (int i = 0; i < depth; ++i )
        printf(i != depth - 1  ? "│   "   : "└───");

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
        switch (errno)
        {
            case EACCES:
                printf("Запрещен поиск в каталоге %s!\n",
                        filename);
                break;
            case EBADF:
                printf("Неверный открытый файловый дескриптор %s!\n",
                        filename);
                break;
            case EFAULT:
                printf("Неверный адрес: %s!\n", filename);
                break;
            case ELOOP:
                printf("Много символьных ссылок при определении пути: %s!\n",
                        filename);
                break;
            case ENAMETOOLONG:
                printf("Слишком длинное значение аргумента: %s!\n", filename);
                break;
            case ENOENT:
                printf("Не существует компонент пути или пустая строка: %s!\n",
                        filename);
                break;
            case ENOMEM:
                printf("Не хватает памяти: %s\n!", filename);
                break;
            case ENOTDIR:
                printf("Компонент в префиксе не является каталогом: %s!\n",
                        filename);
                break;
            case EOVERFLOW:
                printf("Размер файла, номер inode или количество блоков не\
                        могут быть представлены в виде стандартных типов\
                        %s!\n", filename);
                break;
            default:
                printf("Ошибка при получении информации о файле %s!\n",
                        filename);
        }

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
        return 0;
    }

    if (chdir(filename) == -1)
    {
        closedir(dp);
        puts("Ошибка при изменении текущего рабочего каталога!");
        return 0;
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
    char name[NAME_MAX + 1];
    char *buf= name;

    int depth = 0;
    int error = 0;
    node_t *stack = NULL;
    push(&stack, pathname);

    while (!is_empty(&stack) && !error)
    {
        pop(&stack, &buf);
        error = dopath(buf, &depth, &stack);
    }
    
    return 0;
}
