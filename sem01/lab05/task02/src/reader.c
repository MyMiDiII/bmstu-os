#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "reader.h"
#include "constants.h"

struct sembuf reader_begin[5] = 
{
	{CAN_READ, 1, 0},         
	{WAIT_WRITERS, 0, 0},
	{CAN_WRITE, 0, 0},
	{ACTIVE_READERS, 1, 0},
	{CAN_READ, -1, 0}
};

struct sembuf reader_release[1] = 
{
	{ACTIVE_READERS, -1, 0}
};


int start_read(int sid) 
{
    return semop(sid, reader_begin, 5);
}

int stop_read(int sid) 
{
    return semop(sid, reader_release, 1);
}

void run_reader(int *const counter, const int sid, const int rid)
{
	int sleep_time = rand() % READER_SLEEP_TIME + 1;
	sleep(sleep_time);

	if (start_read(sid) == -1)
	{
		perror("Something went wrong with start_read!");
		exit(-1);
	}

    // critical section
	printf("\e[1;33mReader #%d \tread: \t%d \tsleep: %d\e[0m \n", 
				rid, *counter, sleep_time);
	// critical section ends
	
	if (stop_read(sid) == -1)
	{
		perror("Something went wrong with stop_read!");

        exit(-1);
	}
}

void create_reader(int *const counter, const int sid, const int rid)
{
	pid_t childpid;
	if ((childpid = fork()) == -1)
	{
		perror("Can't fork!");
		exit(-1);
	}
	
    if (childpid == 0)
	{
        for (int i = 0; i < ITERS_NUM + 2; i++)
			run_reader(counter, sid, rid);

		exit(0);
	}
}
