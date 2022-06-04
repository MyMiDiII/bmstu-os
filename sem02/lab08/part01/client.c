#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "common.h"

int main(void)
{
    int sock;
    struct sockaddr_un addr;

    char msg[BUF_SIZE];

    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        perror("sock");
        return -1;
    }

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_NAME, strlen(SOCK_NAME));

    snprintf(msg, BUF_SIZE, "Message from client with pid = %d\n", getpid());

    if (sendto(sock, msg, strlen(msg) + 1, 0,
                (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("sendto");
        return -1;
    }

    close(sock);
    printf("Send msg:\n%s\n", msg);

    return 0;
}
