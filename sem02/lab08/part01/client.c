#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "colors.h"
#include "common.h"

void cleanup(int sockfd, const char *name)
{
    close(sockfd);
    unlink(name);
}

int main(void)
{
    int sockfd;
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        puts(RED "socket error" RESET);
        return -1;
    }

    struct sockaddr_un cliaddr;
    cliaddr.sun_family = AF_UNIX;
    snprintf(cliaddr.sun_path, sizeof(cliaddr.sun_path),
                CLI_SOCK_NAME, getpid());

    if (bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) == -1)
    {
        puts(RED "bind error" RESET);
        close(sockfd);
        return -1;
    }

    struct sockaddr_un servaddr;
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SERV_SOCK_NAME, strlen(SERV_SOCK_NAME) + 1);

    char msg[BUF_SIZE];
    snprintf(msg, BUF_SIZE, "Message from client with pid = %d", getpid());

    int bytes = sendto(sockfd, msg, strlen(msg), 0,
                          (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (bytes == -1)
    {
        puts(RED "recvfrom error" RESET);
        cleanup(sockfd, cliaddr.sun_path);
        return -1;
    }
    printf(BCYN "Sent message:\n%s%s%s\n", WHT, msg, RESET);

    char buf[BUF_SIZE];
    bytes = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
    if (bytes == -1)
    {
        puts(RED "recvfrom error" RESET);
        cleanup(sockfd, cliaddr.sun_path);
        return -1;
    }
    buf[bytes] = '\0';

    printf(BBLU "Server message:\n%s%s%s\n", WHT, buf, RESET);

    cleanup(sockfd, cliaddr.sun_path);
    return 0;
}

