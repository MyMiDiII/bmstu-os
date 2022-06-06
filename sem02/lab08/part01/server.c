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
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        puts(BRED "socket error" RESET);
        return -1;
    }

    struct sockaddr_un servaddr;
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SERV_SOCK_NAME, strlen(SERV_SOCK_NAME) + 1);

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {
        puts(BRED "bind error" RESET);
        close(sockfd);
        return -1;
    }

    if (signal(SIGINT, sighand) == SIG_ERR)
    {
        puts(BRED "signalerror" RESET);
        cleanup(sockfd);
        return -1;
    }

    char proof[BUF_SIZE];
    strncpy(proof, CONFIRMATION, strlen(CONFIRMATION));

    puts(BGRN "Server is ready!" RESET);

    while (1)
    {
        char buf[BUF_SIZE];
        struct sockaddr_un cliaddr;
        socklen_t addrsize = sizeof(cliaddr);

        int bytes = recvfrom(sockfd, buf, sizeof(buf), 0,
                                (struct sockaddr *)&cliaddr, &addrsize);
        if (bytes == -1)
        {
            puts(BRED "recvfrom error" RESET);
            cleanup(sockfd);
            return -1;
        }
        buf[bytes] = '\0';

        printf(BCYN "\nRecieved message:\n%s%s%s\n", WHT, buf, RESET);

        bytes = sendto(sockfd, proof, strlen(proof), 0,
                         (struct sockaddr *)&cliaddr, addrsize);
        if (bytes == -1)
        {
            puts(BRED "recvfrom error" RESET);
            cleanup(sockfd);
            return -1;
        }

        printf(BBLU "Sent confirmation:\n%s%s%s\n", WHT, proof, RESET);
    }
}
