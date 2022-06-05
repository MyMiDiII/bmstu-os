#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <errno.h>

#include "colors.h"
#include "common.h"

#define CONFIRMATION "Message recieved!" 

static int sockfd;

void cleanup(int sockfd)
{
    close(sockfd);
    unlink(SERV_SOCK_NAME);
}

void sighand(int sig)
{
    cleanup(sockfd);
    exit(0);
}

int main(void)
{
    //setbuf(stdout, NULL);
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        puts(RED "socket error" RESET);
        return -1;
    }

    struct sockaddr_un servaddr;
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SERV_SOCK_NAME, strlen(SERV_SOCK_NAME) + 1);

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {
        puts(RED "bind error" RESET);
        close(sockfd);
        return -1;
    }

    if (signal(SIGINT, sighand) == SIG_ERR)
    {
        puts(RED "signalerror" RESET);
        cleanup(sockfd);
        return -1;
    }

    char proof[BUF_SIZE];
    strncpy(proof, CONFIRMATION, strlen(CONFIRMATION));

    puts(GRN "Server is ready!" RESET);

    while (1)
    {
        char buf[BUF_SIZE];
        struct sockaddr_un cliaddr;
        socklen_t addrsize;

        int bytes = recvfrom(sockfd, buf, sizeof(buf), 0,
                                (struct sockaddr *)&cliaddr, &addrsize);
        if (bytes == -1)
        {
            printf("%d\n", EBADF);
            printf("%d\n", ECONNREFUSED);
            printf("%d\n", ENOTCONN);
            printf("%d\n", ENOTSOCK);
            printf("%d\n", EAGAIN);
            printf("%d\n", EINTR);
            printf("%d\n", EFAULT);
            printf("%d\n", EINVAL);
            printf("%d\n", errno);
            puts(RED "recvfrom error" RESET);
            cleanup(sockfd);
            return -1;
        }
        buf[bytes] = '\0';

        printf(BMAG "Recieved message: %s%s\n", RESET, buf);

        bytes = sendto(sockfd, proof, strlen(proof), 0,
                         (struct sockaddr *)&cliaddr, addrsize);
        if (bytes == -1)
        {
            puts(RED "recvfrom error" RESET);
            cleanup(sockfd);
            return -1;
        }
    }

    return 0;
}
