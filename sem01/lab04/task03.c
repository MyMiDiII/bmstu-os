#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>

#define OK 0
#define ERROR 1

#define SLEEP_TIME 1


void check_status(const int status)
{
    if (WIFEXITED(status))
    {
        printf("Child exited with code %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("Child was terminated by a signal with number %d\n",
               WIFSIGNALED(status));
    }
    else if (WIFSTOPPED(status))
    {
        printf("Child was stopped by a signal with number %d\n",
               WSTOPSIG(status));
    }
}


int main(void)
{
    pid_t first_child_id, second_child_id;

    if ((first_child_id = fork()) == -1)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    else if (!first_child_id)
    {
        printf("FIRST CHILD: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());

        if (execlp("factor", "factor", "273", 0) == -1)
        {
            printf("Can't exec!\n");
            return ERROR;
        }

        return OK;
    }

    if ((second_child_id = fork()) == -1)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    else if (!second_child_id)
    {
        printf("SECOND CHILD: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());

        if (execlp("pwd", "pwd", 0) == -1)
        {
            printf("Can't exec!\n");
            return ERROR;
        }

        return OK;
    }

    printf("PARENT: pid %d, pgrp %d, child1 %d, child2 %d\n\n",
           getpid(), getpgrp(), first_child_id, second_child_id);

    int status;
    pid_t child_pid;

    child_pid = wait(&status);
    printf("\nChild with pid = %d has finished\n", child_pid);
    check_status(status);

    child_pid = wait(&status);
    printf("\nChild with pid = %d has finished\n", child_pid);
    check_status(status);

    return OK;
}
