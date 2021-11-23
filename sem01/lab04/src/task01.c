#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#define OK 0
#define ERROR 1
#define FORK_ERR -1

#define SLEEP_TIME 1

int main(void)
{
    pid_t first_child_id, second_child_id;

    if ((first_child_id = fork()) == FORK_ERR)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    
    if (first_child_id == 0)
    {
        printf("First child before sleep: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());

        sleep(SLEEP_TIME);

        printf("\nFirst child after sleep: pid %d, ppid %d, pgrp %d\n",
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
        printf("Second child before sleep: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());

        sleep(SLEEP_TIME);

        printf("Second child after sleep: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());
        return OK;
    }

    printf("Parent: pid %d, pgrp %d, child1 %d, child2 %d\n\n",
           getpid(), getpgrp(), first_child_id, second_child_id);

    return OK;
}
