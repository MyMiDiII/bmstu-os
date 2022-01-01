#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <signal.h>

#include "constants.h"
#include "rw.h"

int main(void)
{
    signal(SIGINT, SIG_IGN);
	int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int shmid = shmget(IPC_PRIVATE, sizeof(int), perms);
	if (shmid == -1)
	{
		perror("Failed to create shared memory!");
		return -1;
	}

    int *counter = (int*)shmat(shmid, NULL, 0);
	if (counter == (void*)-1)
	{
		perror("Shmat failed!");
		return -1;
	}
    *counter = 0;

    int sem_descr = semget(IPC_PRIVATE, 4, IPC_CREAT | perms);
	if (sem_descr == -1)
	{
		perror("Failed to create semaphores!");
		return -1;
	}

	if (semctl(sem_descr, ACTIVE_READERS, SETVAL, 0) == -1)
	{
		perror("Can't set control semaphors!");
		return -1;
	}

	if (semctl(sem_descr, ACTIVE_WRITERS, SETVAL, 0) == -1)
	{
		perror("Can't set control semaphors!");
		return -1;
	}

	if (semctl(sem_descr, WAITING_WRITERS, SETVAL, 0) == -1)
	{
		perror("Can't set control semaphors!");
		return -1;
	}

	if (semctl(sem_descr, ACCESS, SETVAL, 1) == -1)
	{
		perror("Can't set control semaphors!");
		return -1;
	}

	for (int i = 0; i < WRITERS_NUM; i++)
		create_writer(counter, sem_descr, i + 1);

    for (int i = 0; i < READERS_NUM; i++)
		create_reader(counter, sem_descr, i + 1);

	for (int i = 0; i < READERS_NUM + WRITERS_NUM; i++)
    {
        int status;
        if (wait(&status) == -1) 
		{
            perror("Something wrong with children waiting!");
            return -1;
        }

        if (!WIFEXITED(status))
            printf("One of children terminated abnormally!");
    }

    printf("\e[1;32mOK\e[0m\n");

	if (shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		perror("Failed to mark the segment to be destroyed!");
		return -1;
	}

	if (shmdt((void*)counter) == -1)
	{
		perror("Failed to detach the segment!");
		return -1;
	}

	if (semctl(sem_descr, 0, IPC_RMID, 0) == -1)
	{
		perror("Failed to delete semaphores!");
		return -1;
	}

	return 0;
}

