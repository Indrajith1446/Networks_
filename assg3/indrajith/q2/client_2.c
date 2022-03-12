#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SA struct sockaddr
#define PORT 8080
#define BSIZE 500
#define MAX 80

FILE *f1;

int miscfun(int socket_fd){

	char buff[MAX];
	
	for(;;){
		bzero(&buff,MAX);
		
		printf("\nMessage to server: ");		
		int n=0;
		while((buff[n++] = getchar())!='\n');
		
		write(socket_fd,buff,sizeof(buff));
		
		if(strncmp(buff, "Bye", 3)==0){
			printf("Closing connection by client\n");
			return 0;
		}
		if(strncmp(buff,"GivemeyourVideo",15)==0){
			printf("getting file\n");
			if((f1=fopen("new.txt","w"))==0){
				printf("coudnt create file");
				break;
			}
			char *buffer=(char *)malloc(BSIZE*sizeof(char));
			recv(socket_fd, buffer, BSIZE,0);
			printf("%s",buffer);
			
			while(strncmp(buffer,"$--End--$",9)!=0){
				printf("%s",buffer);
				fprintf(f1,"%s",buffer);
				recv(socket_fd, buffer, BSIZE,0);	
			}
			
			printf("\n--FILE RECEIVED--");
			continue;
		}
		bzero(&buff,MAX);
		
		read(socket_fd, buff, sizeof(buff));
		printf("\nFrom server: %s",buff);

		
	}
	return 0;
}
int main()
{
	int socket_fd,connection_fd;
	struct sockaddr_in server,client;
	//make socket
	socket_fd=socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd==-1){
		printf("Socket creation failed\n");
		return 0;
	}
	else
		printf("Socket created succesfully\n");
	
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);
	
	if(connect(socket_fd,(SA*)&server,sizeof(server))!=0){
		printf("Failed in establishing connection\n");
		return 0;
	}
	else
		printf("Connected to server\n");
		
	miscfun(socket_fd);
	
	close(socket_fd);
	
	return 1;
}
