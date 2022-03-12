#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>

#define SA struct sockaddr
#define PORT 8080

#define MAX 80


void math(char *buffer){
	int n1,n2;
	char op;
	if(!sscanf(buffer,"%d%c%d\n",&n1,&op,&n2)){
		strcpy(buffer,"Please enter in proper format");
		return ;
	}
	switch(op){
		case '+': sprintf(buffer,"%d\n",n1+n2);
			break;
		case '-': sprintf(buffer,"%d\n",n1-n2);
			break;
		case '/': sprintf(buffer,"%d\n",n1/n2);
			break;
		case '*': sprintf(buffer,"%d\n",n1*n2);
			break;
		case '%': sprintf(buffer,"%d\n",n1%n2);
			break;
		default:strcpy(buffer,"Please enter in proper format");
			break;
	}
	return;

}


int main(){
	int socket_fd,connection_fd,len;
	int i,max_client=10,client_socket[10];
	char buffer[MAX];
	int max_sd,sd;
	int activity;
	int opt=1,isread;
	pid_t child_id;
	
	struct sockaddr_in server, client;
	fd_set readfds;
	for(i=0;i<max_client;i++)
		client_socket[i]=0;
	
	
	socket_fd=socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd==-1){
		printf("Socket creation failed\\n");
		return 0;
	}
	else
		printf("Socket created succssfully %d\n",socket_fd);
		
		
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
	
	if(listen(socket_fd,3)<0){
		printf("Listening failed\n");
		return 0;
	}
	else
		printf("Listening... \n");
	
	len=sizeof(server);

	
	for(;;){
		connection_fd= accept(socket_fd, (SA*)&client, (socklen_t*)&len);
		if(connection_fd<0)
			return 0;
		printf("client new: connection_fd %d , ip %s , port %d , \n",connection_fd,inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		if((child_id = fork())==0){
			close(socket_fd);
			for(;;){
				read(connection_fd,buffer,80);
				if(strncmp(buffer,"bye",3)==0){
					send(connection_fd,"bye",sizeof("bye"),0);
					close(connection_fd);
					break;
				}else{
					char* temp=(char *)malloc(MAX*sizeof(char));
					strcpy(temp,buffer);
					math(temp);
					send(connection_fd,temp,sizeof(temp),0);
					bzero(buffer,sizeof(buffer));
				
				}
			}
		}
	}
	
	return 1;
}
