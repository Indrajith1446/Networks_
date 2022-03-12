#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 8080
#define MAX 80

int miscfun(){
	
		
	return 0;
}

int main() {
	int socket_fd;
	char buffer[MAX];
	char *hello = "Hello from client";
	struct sockaddr_in server;

	
	if ( (socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int n, len;
	
	//send
	sendto(socket_fd, (const char *)hello, strlen(hello),
		MSG_CONFIRM, (const struct sockaddr *) &server,
			sizeof(server));
	printf("Hello message sent.\n");
	
	//receive	
	n = recvfrom(socket_fd, (char *)buffer, MAX,
				MSG_WAITALL, (struct sockaddr *) &server,
				&len);
	buffer[n] = '\0';
	printf("Server : %s\n", buffer);
	for(;;){
		bzero(&buffer,MAX);
		
		printf("\nMessage to server: ");		
		int n=0;
		while((buffer[n++] = getchar())!='\n');
		
		sendto(socket_fd, buffer, strlen(buffer),
			MSG_CONFIRM, (const struct sockaddr *) &server,
			sizeof(server));
		
		if(strncmp(buffer, "stop", 4)==0){
			printf("Closing connection by server\n");
			break;
		}
		bzero(&buffer,MAX);
		
		n = recvfrom(socket_fd, (char *)buffer, MAX,
				MSG_WAITALL, (struct sockaddr *) &server,
				&len);
		buffer[n] = '\0';
		printf("\nFrom server: %s",buffer);

		
	}
	close(socket_fd);
	return 0;
}

