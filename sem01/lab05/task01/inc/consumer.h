#ifndef CONSUMER_H
#define CONSUMER_H

#include <sys/sem.h>

#include "constants.h"
#include "buffer.h"

void run_consumer(buffer_t* const buf, const int sid, const int cid);
void create_consumer(buffer_t* const buf, const int sid, const int cid);

#endif
