#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SA struct sockaddr
#define PORT 8080
#define BSIZE 500
#define MAX 80

FILE *f1;


int fun(int connection_fd){
	char buff[MAX];
	
	for(;;){
		bzero(&buff,MAX);
		read(connection_fd, buff, sizeof(buff));
		
		
		
		if(strncmp(buff, "Bye", 3)==0){
			printf("Closing connection by server\n");
			break;
		}
		else if(strncmp(buff,"GivemeyourVideo",15)==0){
			bzero(&buff,MAX);
			if((f1=fopen("bigfile.txt","r"))==NULL){
				printf("No file found");
				break;
			}
			char *buffer=(char*)malloc(BSIZE*sizeof(char));
			fread(buffer,sizeof(char),BSIZE,f1);
			while(getc(f1)!=EOF){
				printf("%s",buffer);
				send(connection_fd,buffer,BSIZE,0);
				fread(buffer,sizeof(char),BSIZE,f1);
				
			}
			send(connection_fd,"$--End--$",BSIZE,0);
			

			/*fread(buffer,sizeof(char),BSIZE,f1);
			send(connection_fd,buffer,BSIZE,0);*/
			
			continue;
					
		}
		else{
			printf("here");
			printf("\nFrom client: %s",buff);
			bzero(&buff,MAX);
			
			strcpy(buff,"Enter something useful");
		}
		write(connection_fd,buff,sizeof(buff));
		
	}
	
	return 0;
}

int main(){
	int socket_fd,connection_fd,len;
	
	struct sockaddr_in server, client;
	socket_fd=socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd==-1){
		printf("Socket creation failed\\n");
		return 0;
	}
	else
		printf("Socket created succssfully\n");
	bzero(&server, sizeof(server));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);
	
	if(bind(socket_fd, (SA*)&server, sizeof(server))!=0){
		printf("Socket binding faliled\n");
		return 0;
	}
	else
		printf("sokcet binded successfully\n");
	
	if(listen(socket_fd,5)!=0){
		printf("Listening failed\n");
		return 0;
	}
	else
		printf("Listening... \n");
	
	len=sizeof(client);
	
	connection_fd=accept(socket_fd, (SA*)&client, &len);
	
	if(connection_fd<0){
		printf("Server accept failed\n");
		return 0;
	}
	else
		printf("Server accept success\n");
	
	fun(connection_fd);
	
	close(socket_fd);
	
	return 1;
}
