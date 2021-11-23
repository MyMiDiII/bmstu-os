#include <stdlib.h>

#include "queue.h"
#include "err.h"

int aq_pop(queue_t *queue, uint *const node)
{
    if (!queue->length)
        return ERR_EMPTY_QUEUE;

    queue->length--;
    *node = queue->array[queue->out];
    queue->out = (queue->out + 1 + queue->capacity) % queue->capacity;
    return OK;
}

int aq_push(queue_t *queue, const uint node)
{
    if (queue->length == queue->capacity)
        return ERR_FULL_QUEUE;

    queue->array[queue->in] = node;
    queue->in = (queue->in + 1 + queue->capacity) % queue->capacity;
    queue->length++;
    return OK;
}

int queue_init(queue_t *queue, const uint num)
{
    queue->array = malloc(sizeof(uint) * num);

    if (!queue->array)
       return ERR_MEMORY;

    queue->capacity = num;
    queue->length = 0;
    queue->in = 0;
    queue->out = 0;

    return OK;
}
