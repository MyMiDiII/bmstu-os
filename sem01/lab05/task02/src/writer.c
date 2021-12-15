#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "constants.h"
#include "writer.h"

struct sembuf writer_begin[5] = 
{
	{WAIT_WRITERS, 1, 0},
	{ACTIVE_READERS, 0, 0},
	{CAN_WRITE, 0, 0},
	{CAN_WRITE, 1, 0},
	{WAIT_WRITERS, -1, 0}
};

struct sembuf writer_release[] =
{
	{CAN_WRITE, -1, 0}
};

int start_write(int sem_id) 
{
    return semop(sem_id, writer_begin, 5);
}

int stop_write(int sem_id) 
{
    return semop(sem_id, writer_release, 1);
}

void run_writer(int *const counter, const int sid, const int wid)
{
	int sleep_time = rand() % WRITER_SLEEP_TIME + 1;
	sleep(sleep_time);

	if (start_write(sid) == -1)
	{
		perror("Something went wrong with start_write!");
		exit(-1);
	}

	// critical section
	(*counter)++;
	printf("\e[1;32mWriter #%d write: %d (sleep: %d)\e[0m\n", 
				wid, *counter, sleep_time);
	// critical section ends

	if (stop_write(sid) == -1)
	{
		perror("Something went wrong with stop_write!");
		exit(-1);
	}
}

void create_writer(int *const counter, const int sid, const int wid)
{
	pid_t childpid;
	if ((childpid = fork()) == -1)
	{
		perror("Can't fork!");
		exit(-1);
	}
	else if (childpid == 0)
	{
		for (int i = 0; i < ITERS_NUM; i++)
			run_writer(counter, sid, wid);

		exit(0);
	}
}
