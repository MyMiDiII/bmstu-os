#include <string.h>

#include "buffer.h"

int init_buffer(buffer_t *const buf)
{
    if (!buf)
        return -1;

    memset(buf, 0, sizeof(buffer_t));

    return 0;
}

int write_buffer(buffer_t *const buf, const char el)
{
    if (!buf)
        return -1;
    
    buf->data[buf->write_pos++] = el;
    buf->write_pos %= N;

    return 0;
}

int read_buffer(buffer_t *const buf, char *const dest)
{
    if (!buf)
        return -1;

    *dest = buf->data[buf->read_pos++];
    buf->read_pos %= N;

    return 0;
}
