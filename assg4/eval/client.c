#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SA struct sockaddr
#define PORT 8182

#define MAX 80
int miscfun(int socket_fd){

	char buff[MAX];
	bzero(&buff,MAX);
	for(;;){
		read(socket_fd, buff, sizeof(buff));
		printf("\nFrom server: %s",buff);
		bzero(&buff,MAX);
		
		printf("\n");		
		int n=0;
		while((buff[n++] = getchar())!='\n');
		
		write(socket_fd,buff,sizeof(buff));
		
		if(strncmp(buff, "bye", 3)==0){
			printf("Closing connection by client\n");
			return 0;
		}
		bzero(&buff,MAX);
		
		
	}
	return 0;
}
int main()
{
	int socket_fd,connection_fd;
	struct sockaddr_in address;
	
	socket_fd=socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd==-1){
		printf("Socket creation failed\n");
		return 0;
	}
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);
	
	if(connect(socket_fd,(SA*)&address,sizeof(address))!=0){
		printf("Failed in establishing connection\n");
		return 0;
	}
	char buff[MAX];
	int n=0;
	bzero(buff,MAX);
	read(socket_fd, buff, sizeof(buff));
	if(strncmp(buff,"pw",2)==0){
		printf("Enter the password:");
		bzero(buff,MAX);
		while((buff[n++] = getchar())!='\n');
		write(socket_fd,buff,sizeof(buff)); 
	}
	bzero(&buff,MAX);
	read(socket_fd, buff, sizeof(buff));
	printf("%s\n",buff);
		
	miscfun(socket_fd);
	
	close(socket_fd);
	
	return 1;
}
