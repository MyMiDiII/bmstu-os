#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "common.h"

static int sock;

void cleanup(int sock)
{
    close(sock);
    unlink(SOCK_NAME);
}

void sighandler(int sig)
{
    cleanup(sock);
    exit(0);
}

int main(void)
{
    setbuf(stdout, NULL);

    struct sockaddr_un addr;

    char buf[BUF_SIZE];
    int bytes;

    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_NAME, strlen(SOCK_NAME));

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(sock);
        return -1;
    }

    if (signal(SIGINT, sighandler) == SIG_ERR)
    {
        perror("signal");
        cleanup(sock);
        return -1;
    }

    puts("Server run");

    while (1)
    {
        if ((bytes = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL)) == -1)
        {
            perror("recvfrom");
            cleanup(sock);
            return -1;
        }
        buf[bytes] = '\0';
        printf("Received message:\n%s\n", buf);

//        struct sockaddr_un cli_addr;
//        socklen_t addr_size = sizeof(cli_addr);
//
//        if ((bytes = recvfrom(sock, buf, sizeof(buf), 0,
//                    (struct sockaddr*)&cli_addr, &addr_size)) == -1)
//        {
//            perror("recvfrom");
//            cleanup(sock);
//            return -1;
//        }
//        buf[bytes] = '\0';
//        printf("Received message:\n%s\n", buf);
//        printf("name %s", cli_addr.sun_path);
//
//        snprintf(buf, BUF_SIZE, "Hello!");
//
//       if (sendto(sock, buf, strlen(buf), 0,
//                   (struct sockaddr *)&cli_addr, addr_size) == -1)
//       {
//           perror("sentto");
//           return -1;
//       }
    }

    return 0;
}
