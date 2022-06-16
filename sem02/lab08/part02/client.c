#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "colors.h"
#include "common.h"

#define MSG_TEMPL "Client pid = %d\nMessage: %s"

static int sockfd;

void sighand(int sig)
{
    puts(RED "\nDisconnected" RESET);
    close(sockfd);
    exit(0);
}

int main(void)
{
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        puts(RED "socket error" RESET);
        return -1;
    }

    struct sockaddr_in servaddr = 
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        puts(RED "connect error" RESET);
        close(sockfd);
        return -1;
    }

    if (signal(SIGINT, sighand) == SIG_ERR)
    {
        puts(BRED "signal error" RESET);
        close(sockfd);
        return -1;
    }

    while(1)
    {
        char msg[BUF_SIZE / 2];
        printf("Enter message to server > ");

        if (fgets(msg, BUF_SIZE / 2, stdin) == NULL)
        {
            puts(RED "fgets error" RESET);
            close(sockfd);
            return -1;
        }
        msg[strlen(msg) - 1] = '\0';

        char full_msg[BUF_SIZE];
        snprintf(full_msg, BUF_SIZE, MSG_TEMPL, getpid(), msg);

        int bytes = send(sockfd, full_msg, strlen(full_msg), 0);
        if (bytes == -1)
        {
            puts(RED "send error" RESET);
            close(sockfd);
            return -1;
        }
        printf(BMAG "Sent message:\n%s%s%s\n", WHT, full_msg, RESET);

        char buf[BUF_SIZE];
        bytes = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        if (bytes == -1)
        {
            puts(RED "recvfrom error" RESET);
            close(sockfd);
            return -1;
        }
        else if (bytes == 0)
        {
            puts(RED "Server is disconnected" RESET);
            close(sockfd);
            return -1;
        }
        buf[bytes] = '\0';

        printf(BBLU "Server message:\n%s%s%s\n", WHT, buf, RESET);
    }
}
