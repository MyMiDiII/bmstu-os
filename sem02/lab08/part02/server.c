#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "common.h"
#include "colors.h"

#define MAX_SOCK_NUM 3
#define MAX_SOCK_LEN MAX_SOCK_NUM + 1
#define CONFIRMATION "Message recieved!" 

static int sockfd;

void sighand(int sig)
{
    close(sockfd);
    exit(0);
}

int main(void)
{
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        puts(BRED "socket error" RESET);
        return -1;
    }

    struct sockaddr_in servaddr = 
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {
        puts(BRED "bind error" RESET);
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 1) == -1)
    {
        puts(BRED "listen error" RESET);
        close(sockfd);
        return -1;
    }

    struct pollfd sock_set[MAX_SOCK_LEN];
    sock_set[0].fd = sockfd;
    sock_set[0].events = POLLIN;
    sock_set[0].revents = 0;
    int cur_num = 1;

    char proof[BUF_SIZE];
    strncpy(proof, CONFIRMATION, strlen(CONFIRMATION) + 1);
    int first_over = 1;

    puts(BGRN "Server is ready!" RESET);

    while (1)
    {
        int true_num = poll(sock_set, cur_num, -1);

        if (true_num == -1)
        {
            puts(BRED "poll error" RESET);
            close(sockfd);
            return -1;
        }

        if (cur_num < MAX_SOCK_LEN && (sock_set[0].revents & POLLIN))
        {
            puts(BGRN "New connection" RESET);

            int new_sock;
            if ((new_sock = accept(sock_set[0].fd, NULL, NULL)) == -1)
            {
                puts(BRED "accept error" RESET);
                close(sockfd);
                return -1;
            }

            sock_set[cur_num].fd = new_sock;
            sock_set[cur_num].events = POLLIN;
            sock_set[cur_num].revents = 0;

            printf(BGRN "Client #%d connected%s\n", cur_num, RESET);
            cur_num++;
        }
        else if (first_over && (cur_num == MAX_SOCK_LEN))
        {
            puts(BYEL "No more sockets for clients" RESET);
            first_over = 0;
        }

        for (int i = 1; i < cur_num; ++i)
        {
            if (sock_set[i].revents & POLLIN)
            {
                char msg[BUF_SIZE];
                int bytes = recv(sock_set[i].fd, msg, sizeof(msg), 0);

                if (bytes < 1)
                {
                    printf("Client #%d is disconnected\n", i);
                    close(sock_set[i].fd);
                    if (i < cur_num - 1)
                        sock_set[i] = sock_set[cur_num - 1];
                    i--;
                    cur_num--;
                    first_over = 1;
                }
                else
                {
                    msg[bytes] = '\0';
                    printf(BCYN "Recieved message by client #%d:\n%s%s%s\n",
                                i, WHT, msg, RESET);

                    bytes = send(sock_set[i].fd, proof, strlen(proof), 0);
                    if (bytes == -1)
                        puts(BRED "send error" RESET);
                    else
                        printf(BBLU "Sent confirmation:\n%s%s%s\n",
                                    WHT, proof, RESET);
                }
            }
        }
    }

    return 0;
}
