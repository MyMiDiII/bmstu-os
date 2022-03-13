#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>

typedef struct node node_t;

struct node
{
    char *name;
    node_t *prev;
};

_Bool is_empty(node_t **stack);
int push(node_t **stack, char *name);
int pop(node_t **stack, char **name);

#endif
