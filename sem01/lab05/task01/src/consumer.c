#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#include "consumer.h"
#include "buffer.h"

struct sembuf consumer_lock[2] = 
{
	{BUF_FULL, -1, 0},
	{BIN_SEM, -1, 0}
};

struct sembuf consumer_release[2] = 
{
	{BIN_SEM, 1, 0},
	{BUF_EMPTY, 1, 0}
};

void run_consumer(buffer_t* const buf, const int sid, const int cid)
{
	int sleep_time = rand() % CONSUMER_DELAY_TIME + 1;
	sleep(sleep_time);

	if (semop(sid, consumer_lock, 2) == -1)
	{
		perror("Something went wrong with consumer lock!");
		exit(-1);
	}

    char ch;
    if (read_buffer(buf, &ch) == -1) 
	{
        perror("Can't read!");
        exit(-1);
    }
    printf("\e[1;34mConsumer #%d  read: %c (sleep: %d)\e[0m\n", cid,
            ch, sleep_time);

	if (semop(sid, consumer_release, 2) == -1)
	{
		perror("Something went wrong with consumer release!");
		exit(-1);
	}
}

void create_consumer(buffer_t* const buffer, const int sid, const int cid)
{
	pid_t childpid;
	if ((childpid = fork()) == -1)
	{
		perror("Can't fork!");
		exit(-1);
	}
	
    if (childpid == 0)
	{
		for (int i = 0; i < ITERS_NUM; i++)
			run_consumer(buffer, sid, cid);

		exit(0);
	}
}
