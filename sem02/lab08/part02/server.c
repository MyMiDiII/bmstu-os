#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>

#include "common.h"

#define MAX_SOCK_NUM 10

int main(void)
{
    int sock_arr[MAX_SOCK_NUM] = {0};

    struct sockaddr_in addr = 
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    fd_set set;
    int sock, max_sock, new_sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }


    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 1) == -1)
    {
        perror("listen");
        return -1;
    }

    puts("Server is ready");

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(sock, &set);

        max_sock = sock;
        for (int i = 0; i < MAX_SOCK_NUM; ++i)
        {
            int cur_sock = sock_arr[i];

            if (cur_sock > 0)
            {
                FD_SET(cur_sock, &set);
                max_sock = cur_sock > max_sock ? cur_sock : max_sock;
            }
        }

        if (select(max_sock + 1, &set, NULL, NULL, NULL) == -1)
        {
            perror("select");
            return -1;
        }

        if (FD_ISSET(sock, &set))
        {
            puts("New connection");

            if ((new_sock = accept(sock, NULL, NULL)) == -1)
            {
                perror("accept");
                return -1;
            }

            int flag = 1;

            for (int i = 0; i < MAX_SOCK_NUM && flag; ++i)
            {
                if (sock_arr[i] == 0)
                {
                    sock_arr[i] = new_sock;
                    printf("Client #%d\n", i);
                    flag = 0;
                }
            }

            if (flag)
                puts("Max sockets num");
        }

        for (int i = 0; i < MAX_SOCK_NUM; ++i)
        {
            if (sock_arr[i] && FD_ISSET(sock_arr[i], &set))
            {
                char msg[BUF_SIZE];
                int bytes;

                if ((bytes = recvfrom(sock_arr[i], msg,
                                sizeof(msg), 0, NULL, NULL)) == 0)
                {
                    printf("Client #%d is disconnected\n", i);
                    close(sock_arr[i]);
                    sock_arr[i] = 0;
                }
                else
                {
                    msg[bytes] = '\0';
                    printf("Msg from client #%d: %s\n", i, msg);
                }
            }
        }
    }

    close(sock);
    return 0;
}
