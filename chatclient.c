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

#define MAX_MSG_LEN 512
#define MAX_USR_NAME 11
//resolves ips v4/v6
void * GetInAddr(struct sockaddr* sa);
//gets current user handle
void UserSetup(char * usrbuf, int a);
//sets up connection to chatserve
int ConnSetup(char* host, char* port);
//connection loop runs until \quit is entered or server disconnect/quits
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
	//make sure all command line args are present
	if(a != 3){
		fprintf(stderr, "Usage: chatclient {hostname} {port}\n");
		exit(1);
	}else{
		//keep trying to get a user name that is 10 charachters
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
	int c;
	while(1){
		//set up next round of communication
		//clear buffer, print handle to console and add handle to buffer
		memset(buf, 0, sizeof buf);
		printf("%s", handle);
		memcpy(buf, handle, strlen(handle));
		//save a pointer to the start of the actual message
		char* message_start = (buf + strlen(handle));
		char in;
		int count = 0;
		//get single chars while possible
		while((in = getchar()) != '\n' && count < MAX_MSG_LEN-12) {
        	buf[count+strlen(handle)]=in;
        	count++;
    	}
    	//if they are still entering chars discard any that make the message over 500 chars
    	if(count==MAX_MSG_LEN-12){
    		while((in = getchar()) != '\n') {
    		}
    	}
    	buf[count+strlen(handle)+1]='\n';
    	//check for quit statement
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
		//fgets(buf, MAX_MSG_LEN-1, stdin);
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










