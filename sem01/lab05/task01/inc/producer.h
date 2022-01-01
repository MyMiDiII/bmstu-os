#ifndef PRODUCER_H
#define PRODUCER_H

#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include "constants.h"
#include "buffer.h"

void create_producer(buffer_t* const buf, char *const ch,
                     const int sid, const int pdid);

#endif
