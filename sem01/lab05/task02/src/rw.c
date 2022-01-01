#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#include "constants.h"
#include "rw.h"

_Bool flag = true;

void switch_mode(int signal)
{
    flag = false;
}

struct sembuf writer_begin[6] = 
{
    {WAITING_WRITERS, 1, 0},
    {ACTIVE_READERS, 0, 0},
    {ACTIVE_WRITERS, 0, 0},
    {ACTIVE_WRITERS, 1, 0},
    {ACCESS, -1, 0},
    {WAITING_WRITERS, -1, 0}
};

struct sembuf writer_release[2] =
{
    {ACTIVE_WRITERS, -1, 0},
	{ACCESS, 1, 0}
};

struct sembuf reader_begin[3] = 
{
    {ACTIVE_WRITERS, 0, 0},
    {WAITING_WRITERS, 0, 0},
    {ACTIVE_READERS, 1, 0},
};

struct sembuf reader_release[1] = 
{
    {ACTIVE_READERS, -1, 0}
};

int start_write(int sem_id) 
{
    return semop(sem_id, writer_begin, 6);
}

int stop_write(int sem_id) 
{
    return semop(sem_id, writer_release, 2);
}

int start_read(int sid) 
{
    return semop(sid, reader_begin, 3);
}

int stop_read(int sid) 
{
    return semop(sid, reader_release, 1);
}

void run_writer(int *const counter, const int sid, const int wid)
{
    srand(time(NULL) + wid);
	int sleep_time = rand() % WRITER_SLEEP_TIME + 1;
	sleep(sleep_time);

	if (start_write(sid) == -1)
	{
		perror("Something went wrong with start_write!");
		exit(-1);
	}

	(*counter)++;
	printf("\e[1;34mWriter #%d write: %2d (sleep: %d)\e[0m\n", 
				wid, *counter, sleep_time);

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
        signal(SIGINT, switch_mode);

        while(flag)
			run_writer(counter, sid, wid);

		exit(0);
	}
}

void run_reader(int *const counter, const int sid, const int rid)
{
    srand(time(NULL) + rid);
	int sleep_time = rand() % READER_SLEEP_TIME + 1;
	sleep(sleep_time);

	if (start_read(sid) == -1)
	{
		perror("Something went wrong with start_read!");
		exit(-1);
	}

	printf("\e[1;33mReader #%d  read: %2d (sleep: %d)\e[0m \n", 
				rid, *counter, sleep_time);
	
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
        signal(SIGINT, switch_mode);

        while(flag)
			run_reader(counter, sid, rid);

		exit(0);
	}
}
