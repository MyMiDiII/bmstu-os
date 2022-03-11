#include <stdio.h>

#include "stack.h"

int myftw(char *pathname)
{
    node_t *mystack = NULL;
    char name[MAX_FILE_NAME] = "holoso";
    char *dinname = name;

    if (pop(&mystack, &dinname))
        puts("all is ok, stack is empty");

    push(&mystack, "hello");
    push(&mystack, "");
    push(&mystack, "what did you say?");
    pop(&mystack, &dinname);
    printf("%s\n", name);

    pop(&mystack, &dinname);
    printf("%s\n", name);

    pop(&mystack, &dinname);
    printf("%s\n", name);

    pop(&mystack, &dinname);
    printf("%s\n", name);

    if (pop(&mystack, &dinname))
        puts("all is ok, stack is empty");

    puts("Та-дам! Дерево каталогов!");
    
    return 0;
}
