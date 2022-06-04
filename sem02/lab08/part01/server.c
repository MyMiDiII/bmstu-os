// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>

#include "common.h"
	
#define PORT	 8080
#define MAXLINE 1024
	
// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_un servaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SOCK_NAME, strlen(SOCK_NAME));
		
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
		
	int len, n;
	
	len = sizeof(cliaddr); //len is value/result
	
    while (1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                    0, ( struct sockaddr *) &cliaddr,
                    &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);
        sendto(sockfd, (const char *)hello, strlen(hello),
            0, (const struct sockaddr *) &cliaddr,
                len);
        printf("Hello message sent.\n");
    }
		
	return 0;
}



//#include <stdio.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/un.h>
//#include <unistd.h>
//#include <signal.h>
//#include <stdlib.h>
//
//#include "common.h"
//
//static int sock;
//
//void cleanup(int sock)
//{
//    close(sock);
//    unlink(SOCK_NAME);
//}
//
//void sighandler(int sig)
//{
//    cleanup(sock);
//    exit(0);
//}
//
//int main(void)
//{
//    setbuf(stdout, NULL);
//
//    struct sockaddr_un addr;
//
//    char buf[BUF_SIZE];
//    int bytes;
//
//    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
//    {
//        perror("socket");
//        return -1;
//    }
//
//    addr.sun_family = AF_UNIX;
//    strncpy(addr.sun_path, SOCK_NAME, strlen(SOCK_NAME));
//
//    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
//    {
//        perror("bind");
//        close(sock);
//        return -1;
//    }
//
//    if (signal(SIGINT, sighandler) == SIG_ERR)
//    {
//        perror("signal");
//        cleanup(sock);
//        return -1;
//    }
//
//    puts("Server run");
//
//    while (1)
//    {
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
//        if (sendto(sock, buf, strlen(buf), 0,
//                    (struct sockaddr *)&cli_addr, addr_size) == -1)
//        {
//            perror("sentto");
//            return -1;
//        }
//    }
//
//    return 0;
//}
