#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SA struct sockaddr
#define PORT 8080
#define BSIZE 100
#define MAX 80
#define msize 80

long int seqno=1,ackno =0;

FILE *f1;

long int getseq(){
	return seqno++;
}
long int getack(){
	return ackno;
}	
void addseq(char * buff){
	int v1=getseq(),v2=getack();
	sprintf(buff,"%-9d %-9d ",v1,v2);
	return;
}

int retseq(char *buff){
	char s[10];
	int sno;
	for(int i=0;i<9;i++)
		s[i]=buff[i];
	sscanf(s,"%d",&sno);
	return sno;
}
int retack(char *buff){
	char a[10];
	int ano;
	for(int i=10;i<19;i++)
		a[i-10]=buff[i];
	sscanf(a,"%d",&ano);
	return ano;
}
void retmsg(char *buf,char *ret){
	for(int i=20;i<BSIZE;i++)
		ret[i-20]=buf[i];
}


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
				printf("couldn't create file");
				break;
			}
			char *buffer=(char *)malloc(BSIZE*sizeof(char));
			recv(socket_fd, buffer, BSIZE,0);
			
			
			while(strncmp(buffer,"$--End--$",9)!=0){
			
				char * ret=(char *)malloc(msize*sizeof(char));
				int sno=0,ano=0;
				sno=retseq(buffer);
				ano=retack(buffer);
				ackno=sno;
				retmsg(buffer,ret);
				printf("s-%d a-%d %s\n",sno,ano,ret);
				printf("#%ld #%d- ",seqno,ano);
				if(seqno==ano+1)
					fprintf(f1,"%s",ret);
				
				char *ackpk=(char *)malloc(BSIZE*sizeof(char));
				addseq(ackpk);
				strcat(ackpk,"ACK");
				send(socket_fd,ackpk,BSIZE,0);
				
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
