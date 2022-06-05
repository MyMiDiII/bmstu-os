#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#include "common.h"

static int sfd;

void cleanup_socket(int sockfd)
{
    close(sockfd);

    char full_path[BUF_SIZE] = {0};
    strcpy(full_path, SOCKET_PATH);
    strcat(full_path, SOCKET_NAME);
    printf("cleanup fullname = %s\n", full_path);
    unlink(full_path);
}

void sighandler(int signum)
{
    cleanup_socket(sfd);
    exit(0);
}

int main(void) {
    if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket call error");
        return errno;
    }

    struct sockaddr_un sock_addr;
    sock_addr.sun_family = AF_UNIX;

    char full_path[BUF_SIZE] = {0};
    strcpy(full_path, SOCKET_PATH);
    strcat(full_path, SOCKET_NAME);
    printf("fullname = %s\n", full_path);

    strcpy(sock_addr.sun_path, full_path);
    if (bind(sfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1)
    {
        perror("bind call error");
        cleanup_socket(sfd);
        return errno;
    }

    if (signal(SIGINT, sighandler) == SIG_ERR)
    {
        perror("signal call error");
        cleanup_socket(sfd);
        return errno;
    }

    char msg[BUF_SIZE];
    char back[BUF_SIZE];
    for (;;)
    {
        printf("Waiting for client\n");
        struct sockaddr_un client_addr;
        int addr_size;

        size_t bytes = recvfrom(sfd, msg, sizeof(msg) - 1, 0, (struct sockaddr*)
        &client_addr, (socklen_t*)&addr_size);
        if (bytes == -1)
        {
            perror("recv call error");
            cleanup_socket(sfd);
            return errno;
        }
        msg[bytes] = '\0';
        printf("received message from %s: '%s'\n", client_addr.sun_path, msg);
        strcpy(back, msg);

        strcat(back, "//ReceivedSuccess");
        bytes = sendto(sfd, back, strlen(back), 0, (struct sockaddr*)
        &client_addr, addr_size);
        printf("sent message with size %ld: '%s' \n", bytes, back);

        if (bytes <= 0)
        {
            printf("sendto error");
            cleanup_socket(sfd);
            return errno;
        }
    }
}
