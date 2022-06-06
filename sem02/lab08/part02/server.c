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

    struct pollfd sock_set[MAX_SOCK_NUM];
    sock_set[0].fd = sockfd;
    sock_set[0].events = POLLIN;
    sock_set[0].revents = 0;
    int cur_num = 1;

    char proof[BUF_SIZE];
    strncpy(proof, CONFIRMATION, strlen(CONFIRMATION) + 1);

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

        if (sock_set[0].revents & POLLIN)
        {
            puts(BGRN "New connection" RESET);

            int new_sock;
            if ((new_sock = accept(sock_set[0].fd, NULL, NULL)) == -1)
            {
                puts(BRED "accept error" RESET);
                close(sockfd);
                return -1;
            }

            if (cur_num < MAX_SOCK_NUM)
            {
                sock_set[cur_num].fd = new_sock;
                sock_set[cur_num].events = POLLIN;
                sock_set[cur_num].revents = 0;

                printf(BGRN "Client #%d connected%s\n", cur_num, RESET);
                cur_num++;
            }
            else
            {
                puts(BYEL "No more sockets for clients" RESET);
                close(new_sock);
            }
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
                        sock_set[i--] = sock_set[cur_num-- - 1];
                }
                else
                {
                    msg[bytes] = '\0';
                    printf("Recieved message by client #%d: %s\n", i, msg);

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


//FD_ZERO(&set);
//FD_SET(sock, &set);
//
//max_sock = sock;
//for (int i = 0; i < MAX_SOCK_NUM; ++i)
//{
//    int cur_sock = sock_arr[i];
//
//    if (cur_sock > 0)
//    {
//        FD_SET(cur_sock, &set);
//        max_sock = cur_sock > max_sock ? cur_sock : max_sock;
//    }
//}
//
//if (select(max_sock + 1, &set, NULL, NULL, NULL) == -1)
//{
//    perror("select");
//    return -1;
//}
//
//if (FD_ISSET(sock, &set))
//{
//    puts("New connection");
//
//    if ((new_sock = accept(sock, NULL, NULL)) == -1)
//    {
//        perror("accept");
//        return -1;
//    }
//
//    int flag = 1;
//
//    for (int i = 0; i < MAX_SOCK_NUM && flag; ++i)
//    {
//        if (sock_arr[i] == 0)
//        {
//            sock_arr[i] = new_sock;
//            printf("Client #%d\n", i);
//            flag = 0;
//        }
//    }
//
//    if (flag)
//        puts("Max sockets num");
//}
//
//for (int i = 0; i < MAX_SOCK_NUM; ++i)
//{
//    if (sock_arr[i] && FD_ISSET(sock_arr[i], &set))
//    {
//        char msg[BUF_SIZE];
//        int bytes;
//
//        if ((bytes = recvfrom(sock_arr[i], msg,
//                        sizeof(msg), 0, NULL, NULL)) == 0)
//        {
//            printf("Client #%d is disconnected\n", i);
//            close(sock_arr[i]);
//            sock_arr[i] = 0;
//        }
//        else
//        {
//            msg[bytes] = '\0';
//            printf("Msg from client #%d: %s\n", i, msg);
//        }
//    }
//}
