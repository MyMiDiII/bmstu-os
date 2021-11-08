#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#define OK 0
#define ERROR 1

#define SLEEP_TIME 1

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
        sleep(SLEEP_TIME);
        printf("\nFIRST CHILD: pid %d, ppid: %d, pgrp: %d\n",
               getpid(), getppid(), getpgrp());
        return OK;
    }

    if ((second_child_id = fork()) == -1)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    else if (!second_child_id)
    {
        sleep(SLEEP_TIME);
        printf("\nSECOND CHILD: pid %d, ppid: %d, pgrp: %d\n",
               getpid(), getppid(), getpgrp());
        return OK;
    }

    printf("PARENT: pid: %d, pgrp: %d, child1: %d, child2: %d\n",
           getpid(), getpgrp(), first_child_id, second_child_id);

    return OK;
}
