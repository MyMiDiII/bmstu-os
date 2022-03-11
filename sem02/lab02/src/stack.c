#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "stack.h"

_Bool is_empty(node_t **stack)
{
    return (!(*stack));
}

node_t *create_node(char *name)
{
    node_t *node = malloc(sizeof(node_t));

    if (!node)
        return NULL;

    node->name = malloc(sizeof(char) * strlen(name));
    strcpy(node->name, name);
    node->prev = NULL;

    return node;
}


int push(node_t **stack, char *name)
{
    node_t *new_node = create_node(name);

    if (!new_node)
        return -1;

    new_node->prev = *stack;
    *stack = new_node;

    return 0;
}

int pop(node_t **stack, char **name)
{
    if (is_empty(stack))
    {
        strcpy(*name, "");
        return -1;
    }

    node_t *elem = *stack;
    *stack = (*stack)->prev;

    strcpy(*name, elem->name);
    free(elem->name);
    free(elem);

    return 0;
}
