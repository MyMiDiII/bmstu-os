#ifndef BUFFER_H
#define BUFFER_H

#include "constants.h"

typedef struct
{
    char data[N];
    size_t read_pos;
    size_t write_pos;
} buffer_t;

int init_buffer(buffer_t *const buf);
int write_buffer(buffer_t *const buf, const char el);
int read_buffer(buffer_t *const buf, char *const dest);

#endif
