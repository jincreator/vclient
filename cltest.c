#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF	1024

int main(int argc, char **argv)
{
    struct sockaddr_in serveraddr;
    int server_sockfd;
    int client_len;
    char buf[MAXBUF];
    char rbuf[MAXBUF];


    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("165.194.17.27");
    serveraddr.sin_port = htons(atoi("25565"));

    client_len = sizeof(serveraddr);

int i;
while(1){

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len);
    
    memset(buf, 0x00, MAXBUF);
    buf[0]=0xfe;

    write(server_sockfd, buf, 1);

    memset(rbuf, 0x00, MAXBUF);
    read(server_sockfd, rbuf, MAXBUF);

    close(server_sockfd);
    usleep(1000000/400);
}

    printf("read : %s", buf);
}

