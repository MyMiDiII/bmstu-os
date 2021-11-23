#ifndef __QUEUE_H__

#define __QUEUE_H__

#include "io.h"

typedef struct
{
    uint *array;
    uint capacity;
    uint length;
    uint in;
    uint out;
} queue_t;

int aq_pop(queue_t *queue, uint *const node);

int aq_push(queue_t *queue, const uint node);

int queue_init(queue_t *queue, const uint num);

#endif
