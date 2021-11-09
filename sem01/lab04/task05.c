#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include <string.h>

#define OK 0
#define ERROR 1

#define WAIT_SIGNAL_TIME 5

#define TEXT1 "Message 1"
#define TEXT2 "Hello from child!"

#define BUF_SIZE 32

#define NOT_WRITE 0
#define WRITE 1

int mode = NOT_WRITE;

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


void switch_mode(int signal)
{
    mode = WRITE;
}


int main(void)
{
    pid_t first_child_id, second_child_id;

    int fd[2];

    if (pipe(fd) == -1)
    {
        printf("Can't pipe!\n");
        return ERROR;
    }

    if ((first_child_id = fork()) == -1)
    {
        perror("Can't fork!\n");
        return ERROR;
    }
    else if (!first_child_id)
    {
        signal(SIGINT, switch_mode);
        printf("FIRST CHILD: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());

        sleep(WAIT_SIGNAL_TIME);

        if (mode)
        {
            close(fd[0]);
            write(fd[1], TEXT1, strlen(TEXT1) + 1);
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
        signal(SIGINT, switch_mode);
        printf("SECOND CHILD: pid %d, ppid %d, pgrp %d\n",
               getpid(), getppid(), getpgrp());

        sleep(WAIT_SIGNAL_TIME);

        if (mode)
        {
            close(fd[0]);
            write(fd[1], TEXT2, strlen(TEXT2) + 1);
        }

        return OK;
    }

    printf("PARENT: pid %d, pgrp %d, child1 %d, child2 %d\n\n",
           getpid(), getpgrp(), first_child_id, second_child_id);

    signal(SIGINT, 1);

    char buf1[BUF_SIZE] = "\0";
    char buf2[BUF_SIZE] = "\0";

    close(fd[1]);

    read(fd[0], buf1, BUF_SIZE);
    printf("\nText1: %s\n", buf1);

    read(fd[0], buf2, BUF_SIZE);
    printf("Text2: %s\n", strlen(buf2) ? buf2 : buf1 + strlen(buf1) + 1);

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
