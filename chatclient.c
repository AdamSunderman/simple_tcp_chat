#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MSG_LEN 501
#define MAX_USR_NAME 11

void * GetInAddr(struct sockaddr* sa);
void UserSetup(char * usrbuf, int a);
int ConnSetup(char* host, char* port);
void ConnLoop(int sock_fd, char* handle);


int main(int argc, char* argv[]){
	int curr_sock;
	char userhandle[50];
	memset(userhandle, 0, sizeof userhandle);

	// Check program arguments and setup user handle
	UserSetup(userhandle, argc);
	// Setup desired connection
	curr_sock = ConnSetup(argv[1], argv[2]);
    // Continue connection until \quit
	ConnLoop(curr_sock, userhandle);
	close(curr_sock);
	return 0;
}




void * GetInAddr(struct sockaddr* sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void UserSetup(char* usrbuf, int a){
	if(a != 3){
		fprintf(stderr, "Usage: chatclient {hostname} {port}\n");
		exit(1);
	}else{
		for(;;){
			printf("\nEnter your handle (10 charachters max)> ");
			fgets(usrbuf, 50, stdin);
			if(strlen(usrbuf) <= MAX_USR_NAME){
				//printf("\n");
				break;
			}
			//printf("\n");
		}
		int index = strlen(usrbuf);
		usrbuf[index-1] = '>';
		usrbuf[index] = ' ';
		usrbuf[index+1] = '\0';
	}
	
}

int ConnSetup(char* host, char* port){
	int sockfd;
	struct addrinfo hints, *serverinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	// Setup addr structs
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if((rv = getaddrinfo(host, port, &hints, &serverinfo)) != 0){
		fprintf(stderr, "Client [getaddrinfo error] %s\n", gai_strerror(rv));
		exit(1);
	}
	// Attempt to connect
	for(p = serverinfo; p != NULL; p = p->ai_next){
		// Attempt to setup socket
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("Client [Socket error]");
			continue;
		}
		// Attempt to connect socket
		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("Client [Connect error]");
			continue;
		}
		break;
	}
	if(p == NULL){
		fprintf(stderr, "Client: Failed to connect\n");
		return 1;
	}
	inet_ntop(p->ai_family, GetInAddr((struct sockaddr*)p->ai_addr), s, sizeof s);
	printf("Client: Connecting to %s\n\n", s);
	freeaddrinfo(serverinfo);
	return sockfd;
}

void ConnLoop(int sock_fd, char* handle){
	int sent, recieved;
	char buf[MAX_MSG_LEN+51];

	while(1){
		memset(buf, 0, sizeof buf);
		printf("%s", handle);
		memcpy(buf, handle, strlen(handle));
		char* message_start = (buf + strlen(handle));
		fgets((buf + strlen(handle)), MAX_MSG_LEN-1, stdin);
		if(strncmp(message_start, "\\quit", 4) == 0){
			break;
		}
		//send data
		if((sent = send(sock_fd, buf, strlen(buf), 0)) == -1){
			fprintf(stderr, "Client [send failed]\n");
			exit(1);
		}
		else if(sent != strlen(buf)){
			fprintf(stderr, "Client [send bytes off]\n");
			exit(1);
		}
		memset(buf, 0, sizeof buf);
		//recieve data
		if((recieved = recv(sock_fd, buf, MAX_MSG_LEN-1, 0)) == -1){
			fprintf(stderr, "Client: [recieve failed]\n");
			exit(1);
		}
		else if(recieved == 0){
			fprintf(stderr, "Client: [connection closed]\n");
			exit(1);
		}
		printf("%s\n", buf);
	}
}










