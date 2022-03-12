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
#define PORT 8182

#define MAX 80
#define clients 4


int *client_socket;
int isvalid(int no){
	if(client_socket[no]==0)
		return 0;
	return 1;

}

void compose(char *buffer,int sender){
	int no,sd;
	char *person,*msg,*temp;
	person=(char *)malloc(10*sizeof(char));
	temp=(char *)malloc(80*sizeof(char));
	if(!sscanf(buffer,"%s%d : %79[^\n]",person,&no,temp)){
		strcpy(buffer,"Please enter in proper format\n( Person <no> : <msg>)\n");
		return ;
	}
	printf("%s,%d\n",temp,sender);
	if(!isvalid(no)){
		strcpy(buffer,"Person is not valid please choose a valid no from the list\n");
		return ;
	}
	sd=client_socket[no];
	msg=(char *)malloc(100*sizeof(char));
	sprintf(msg,"From Person %d :",sender);
	strcat(msg,temp);
	printf("%s\n",msg);
	send(sd,msg,100,0);
	sprintf(temp,"Message sent to person %d \n",no);
	strcpy(buffer,temp);
	return ;
	

}

void getname(char *name,int pno){
	if(pno==0)
		strcpy(name,"Andrew");
	else if(pno==1)
		strcpy(name,"Stallings");
	else if(pno==2)
		strcpy(name,"Forouzon");
	else
		strcpy(name,"Richard");
	return;
}


int main(){
	int socket_fd,connection_fd,len;
	int i,max_client=4;
	client_socket=(int *)malloc(10*sizeof(int));
	char buffer[MAX];
	int max_sd,sd;
	int activity;
	int opt=1,isread;
	
	struct sockaddr_in server;
	fd_set readfds;
	for(i=0;i<max_client;i++)
		client_socket[i]=0;
	
	
	socket_fd=socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd==-1){
		printf("Socket creation failed\\n");
		return 0;
	}
		
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0){
		printf("Set socket failure\n");
		return 0;
	}	
		
	bzero(&server, sizeof(server));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);
	
	if(bind(socket_fd, (SA*)&server, sizeof(server))!=0){
		printf("Socket binding faliled\n");
		return 0;
	}
	if(listen(socket_fd,3)<0){
		printf("Listening failed\n");
		return 0;
	}
	else
		printf("I am listening on port %d \n",PORT);
	
	len=sizeof(server);
	
	for(;;){
		FD_ZERO(&readfds);
		FD_SET(socket_fd,&readfds);
		max_sd=socket_fd;
		for(i=0;i<max_client;i++){
			int sd=client_socket[i];
			if(sd>0)
				FD_SET(sd,&readfds);
			if(sd>max_sd)
				max_sd=sd;
		}
		
		activity=select( max_sd+1, &readfds, NULL, NULL, NULL);
		if(activity<0 && errno!=EINTR)
			printf("select error");
		
		if(FD_ISSET(socket_fd, &readfds)){
			if((connection_fd= accept(socket_fd,(SA*)&server, (socklen_t*)&len))<0){
				printf("accept error\n");
				return 0;
			}
			
			while(1){
				char *temp=(char *)malloc(100*sizeof(char));
				strcpy(temp,"pw");
				send(connection_fd,temp,strlen(temp),0);
				read(connection_fd,temp,sizeof(temp));
				if(strncmp(temp,"234513",6)==0){
					
					break;
				}else{
					close(connection_fd);
					connection_fd=0;
					break;
				}
				
			
			}
			int pno=0;
			
			for(i=0;i<max_client;i++){
				if(client_socket[i] == 0){
					client_socket[i] = connection_fd;
					pno=i;
					break;
				}
				
			}
			char *name=(char*)malloc(10*sizeof(char));
			getname(name,pno);
			sprintf(buffer,"%s connected\n",name);
			printf("%s",buffer);
			for(i=0;i<max_client;i++){
				if(i!=pno&&client_socket[i]!=0){
					send(client_socket[i],buffer,strlen(buffer),0);
				}
			}
			strcpy(buffer,"Welcome to project cobra\n");
			send(client_socket[pno],buffer,strlen(buffer),0);
			char *temp=(char *)malloc(100*sizeof(char));
				
			int flag=0;		
			strcpy(temp,"The Co-PIs connected are:\n");
			for(int j=0;j<max_client;j++){
				char *msg=(char*)malloc(10*sizeof(char));
				if(client_socket[j]!=0&&pno!=j){
					getname(name,j);
					sprintf(msg,"%d.%s\n",j,name);
					strcat(temp,msg);
					flag=1;
				}
							
			}
			if(flag)
				send(client_socket[pno],temp,strlen(temp),0);
			else{
				strcpy(temp,"No other Co-PIs connected pl wait\n");
				send(client_socket[pno],temp,strlen(temp),0);
			}
			
		}
		for(i=0;i<max_client;i++){
			sd=client_socket[i];
			if(FD_ISSET(sd, &readfds)){
				if((isread= read(sd, buffer, 80))==0){
					getpeername(sd, (SA*)&server, (socklen_t*)&len);
					printf("Disconnected ip %s , port %d \n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
					close(sd);
					client_socket[i]=0;
				}
				else{
					buffer[isread]='\0';
					if(strncmp(buffer,"bye",3)==0){
						printf("disc");
						send(sd,"bye",sizeof("bye"),0);
						close(sd);
						client_socket[i]=0;
					}
					else if(strncmp(buffer,"show contacts",13)==0){
						char *temp=(char *)malloc(100*sizeof(char));
						
						strcpy(temp,"\nContacts:-\n");
						for(int j=0;j<max_client;j++){
							char *msg=(char*)malloc(10*sizeof(char));
							if(client_socket[j]!=0&&i!=j){
								sprintf(msg,"Person %d\n",j);
								strcat(temp,msg);
							}
							
						}
						send(sd,temp,strlen(temp),0);
					}
					else{
						char *temp=(char *)malloc(100*sizeof(char));
						strcpy(temp,buffer);
						printf("%s",temp);
						compose(temp,i);
					
						send(sd, temp, strlen(temp), 0);
					}
				}
			}
		}
	
	}
	
	return 1;
}
