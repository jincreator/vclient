#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define NUM_WORKERS 50
#define PACKETS_SEC 300



int start_worker(struct sockaddr_in addr, int sv)
{
	pid_t pid=fork();
        if(pid<0) {
                perror("worker fork()");
                return -1;
        }
        if(pid>0) {
                return (int)pid;
        }
        run_worker(addr, sv);
        return 0;
}

void run_worker(struct sockaddr_in addr, int sv)
{
	pid_t sid=setsid();
        if(sid==-1) {
                perror("worker setsid()");
		exit(1);
        }

	int s;
	int z;
	char sbuf[512];
	char rbuf[1024];

	int b;
	int optval=1;
	struct sockaddr_in c_addr;
	while(1){

		// set information to bind
		memset(&c_addr, 0x00, sizeof(c_addr) );
		c_addr.sin_family = PF_INET;
		c_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		s = socket(PF_INET, SOCK_STREAM, 0);
		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		
		if( s == -1 ){
			perror("socket()");
			exit(1);
		}

		// bind with port here
		c_addr.sin_port = htons(50000+s);
		b = bind(s, (struct sockaddr*)&c_addr, sizeof(c_addr) );
		if( b == -1 ){
			perror("bind()");
			exit(1);
		}

		int len_inet = sizeof(addr);
		z=connect(s, &addr, len_inet );
		if( z == -1 ){
			perror("connect()");
			exit(1);
		}

		memset(sbuf, 0x00, 512);
		sbuf[0] = 0xfe;
		send(s, sbuf, 1, 0);

		memset(rbuf, 0X00, 1024);
		recv(s, rbuf, 1024, 0);

		close(s);

		usleep(1000000/PACKETS_SEC);
	}

}


int main(int argc, char** argv)
{

	fprintf(stderr, "\nMinecraft 1.2.5 virtual client packet generator\n");

	char address[256];
	char port[256];
	int i;

	fprintf(stderr, "Server Address : ");
	fgets(address, 256, stdin);
	fprintf(stderr, "Server Port : ");
	fgets(port, 256, stdin);

/*
	for(i=0; i<strlen(address); i++){
		if(address[i]=='\n') address[i]='\0';
	}
	for(i=0; i<strlen(port); i++){
		if(address[i]=='\n') address[i]='\0';
	}
*/

	struct sockaddr_in addr;
	memset(&addr, 0x00, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(address);
	if( addr.sin_addr.s_addr == INADDR_NONE ){
		perror("bad address");
		exit(1);
	}

	pid_t *pid_worker=(pid_t *)malloc(sizeof(pid_t)*NUM_WORKERS);
	if(pid_worker==NULL) {
                perror("pid_worker malloc()");
        }
        int *sv_worker=(int *)malloc(sizeof(int)*NUM_WORKERS);
        if(sv_worker==NULL) {
                perror("sv_worker malloc()");
        }

	fprintf(stderr,"\n");
	for(i=0; i<NUM_WORKERS; i++){
		int sv[2];
		if(socketpair(AF_UNIX,SOCK_STREAM,0,sv)!=0) {
                        perror("master socketpair()");
                }
                pid_worker[i]=start_worker(addr, sv[1]);
                sv_worker[i]=dup(sv[0]);
                close(sv[0]);
                close(sv[1]);

		fprintf(stderr, "\rNow constructing client [%d of %d]\t", i+1, NUM_WORKERS);
		usleep(100000);
	}
	fprintf(stderr, "\n Done. %d clients sending %dpackets.\n",NUM_WORKERS, PACKETS_SEC);

	while(1){
		sleep(1);
	}


	return 0;
}
