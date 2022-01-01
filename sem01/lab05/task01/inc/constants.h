#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/stat.h>

#define CONSUMER_DELAY_TIME 4
#define PRODUCER_DELAY_TIME 2

#define N 24
#define ITERS_NUM 8
#define PRODUCER_NUM 3
#define CONSUMER_NUM 3

#define BUF_FULL  0
#define BUF_EMPTY 1
#define BIN_SEM   2

#endif
