#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>

#define MAXLINE 1024

using namespace std;

typedef struct sockaddr SA;

//consumer parameter structure
struct client_parms {
	char* host;
	int port;
};


//function to connect server
int open_clientfd(char *hostname, int port) {
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;

	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientfd < 0) {
		printf("client could not create socket!\n");
		return -1;
	}

	//fill in the server's IP address and port
	hp = gethostbyname(hostname);
	if (hp == NULL) {
		printf("client failed to get host by name!\n");
		return -2;
	}
	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)hp->h_addr_list[0], (char *) &serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = htons(port);
	
	//establish a connection with the server
	if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0) {
		printf("client connect failed!\n");
		return -1;
	}

	return clientfd;

}


//client function
void* client(void* parameters) {

	//get input parameters
	struct client_parms* args = (struct client_parms*) parameters;
	char *host = args->host;
	int port = args->port;
	pthread_t tid = pthread_self();
	int clientfd;

	//each client sends 3 request
	int num = 3;
	char* requests[num];
	char sndMsg1[MAXLINE] = "trip shortest CLV 340";
	char sndMsg2[MAXLINE] = "trip fastest CLV 340";
	char sndMsg3[MAXLINE] = "add road1 v1 v2 0 30 10 0";
	requests[0] = sndMsg1;
	requests[1] = sndMsg2;
	requests[2] = sndMsg3;


	for (int i = 0; i < num; i++) {

		char rcvMsg[MAXLINE];
		clientfd = open_clientfd(host, port);

		//client sends a request to server
		if (send(clientfd, requests[i], strlen(requests[i])+1, 0) < 0) {
			printf("client <%ld> send() failed!\n", tid);
		}
	
		printf("client <%ld> sent a request: %s\n", tid, requests[i]);
	
		//client waits for the reponse of server
		if (recv(clientfd, rcvMsg, MAXLINE, 0) < 0) {
			printf("client <%ld> recv() failed!\n", tid);
		}
		printf("client <%ld> received \"%s\"\n", tid, rcvMsg);

		close(clientfd);

	}
	return NULL;

}






