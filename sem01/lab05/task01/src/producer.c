#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>  

#include "producer.h"
#include "buffer.h"

struct sembuf producer_lock[2] = 
{
	{BUF_EMPTY, -1, 0},
	{BIN_SEM, -1, 0}
};

struct sembuf producer_release[2] = 
{
	{BIN_SEM, 1, 0},
	{BUF_FULL, 1, 0}
};

void run_producer(buffer_t* const buf, const int sid, const int pdid)
{
	int sleep_time = rand() % PRODUCER_DELAY_TIME + 1;
	sleep(sleep_time);

	if (semop(sid, producer_lock, 2) == -1)
	{
		perror("Something went wrong with producer lock!");
		exit(-1);
	}

	const char symb = buf->write_pos + 'a';

    if (write_buffer(buf, symb) == -1) 
    {
        perror("Can't write!");
        exit(-1);
    }

    printf("\e[1;35mProducer #%d write: %c (sleep: %d)\e[0m\n", pdid, symb,
            sleep_time);

	if (semop(sid, producer_release, 2) == -1)
	{
		perror("Something went wrong with producer lock!");
		exit(-1);
	}
}

void create_producer(buffer_t* const buf, const int sid, const int pdid)
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
			run_producer(buf, sid, pdid);

		exit(0);
	}
}
