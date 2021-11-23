#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>

#define OK 0
#define ERROR 1
#define FORK_ERR -1

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
    pid_t first_child_id, second_child_id, child_pid;
    int status;

    if ((first_child_id = fork()) == FORK_ERR)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    
    if (first_child_id == 0)
    {
        sleep(SLEEP_TIME);
        printf("FIRST CHILD: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());
        return OK;
    }

    if ((second_child_id = fork()) == FORK_ERR)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    
    if (second_child_id == 0)
    {
        sleep(SLEEP_TIME);
        printf("SECOND CHILD: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());
        return OK;
    }

    printf("PARENT: pid %d, pgrp %d, child1 %d, child2 %d\n\n",
           getpid(), getpgrp(), first_child_id, second_child_id);

    child_pid = wait(&status);
    printf("\nChild with pid = %d has finished\n", child_pid);
    check_status(status);

    child_pid = wait(&status);
    printf("\nChild with pid = %d has finished\n", child_pid);
    check_status(status);

    return OK;
}
