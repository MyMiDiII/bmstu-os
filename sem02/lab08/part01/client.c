// Client side implementation of UDP client-server model
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
    setbuf(stdout, NULL);
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from client";
	struct sockaddr_un	 servaddr;
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SOCK_NAME, strlen(SOCK_NAME));
		
	int n, len;
		
	sendto(sockfd, (const char *)hello, strlen(hello),
		0, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	printf("Hello message sent.\n");
			
    printf("before");
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, NULL, NULL);
    printf("after");
	buffer[n] = '\0';
	printf("Server : %s\n", buffer);
	
	close(sockfd);
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
//int main(void)
//{
//    int sock;
//    struct sockaddr_un addr;
//
//    char msg[BUF_SIZE];
//
//    if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
//    {
//        perror("sock");
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
//    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr) == -1))
//    {
//        perror("connect");
//        close(sock);
//        return -1;
//    }
//
//    snprintf(msg, BUF_SIZE, "Message from client with pid = %d\n", getpid());
//
//    if (sendto(sock, msg, strlen(msg) + 1, 0,
//                (struct sockaddr *)&addr, sizeof(addr)) == -1)
//    {
//        perror("sendto");
//        return -1;
//    }
//    printf("Send msg:\n%s\n", msg);
//
//    int bytes;
//    if ((bytes = recvfrom(sock, msg, sizeof(msg), 0, NULL, NULL)) == -1)
//    {
//        perror("recvfrom");
//        return -1;
//    }
//    msg[bytes] = '\0';
//    printf("Received message:\n%s\n", msg);
//
//    close(sock);
//
//    return 0;
//}
