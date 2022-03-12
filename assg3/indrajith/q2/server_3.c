#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#define SA struct sockaddr
#define PORT 8080
#define BSIZE 100
#define MAX 80
#define msize 80

long int seqno=1,ackno=0;

FILE *f1;

long int getseq(){
	return seqno++;
}
long int getack(){
	return ackno;
}	
void addseq(char * buff,FILE *f){
	int v1=getseq(),v2=getack();
	char *temp=(char *)malloc(BSIZE*sizeof(char));
	fread(temp,sizeof(char),msize,f);
	sprintf(buff,"%-9d %-9d %s",v1,v2,temp);
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
			if((f1=fopen("tst.txt","r"))==NULL){
				printf("No file found");
				break;
			}
			char *buffer=(char*)malloc(BSIZE*sizeof(char));
			addseq(buffer,f1);
			
			long int ttl=0,i=0,j=0;
			char * commandsForGnuplot[] = {"set title \"Data transmission  rate in Kbps\"", "plot 'data.temp' with linespoints"};
			FILE * temp = fopen("data.temp", "w");
			FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
			
			clock_t before = clock();
			
			while(getc(f1)!=EOF){
				
				int msec=0;
				

				send(connection_fd,buffer,BSIZE,0);
				
				char *ackpk=(char *)malloc(BSIZE*sizeof(char));
				recv(connection_fd, ackpk, BSIZE,0);
				char * ret=(char *)malloc(msize*sizeof(char));
				int sno=0,ano=0;
				sno=retseq(ackpk);
				ano=retack(ackpk);
				
				while(sno!=ackno+1){
					send(connection_fd,buffer,BSIZE,0);
					
					char *ackpk=(char *)malloc(BSIZE*sizeof(char));
					recv(connection_fd, ackpk, BSIZE,0);
					char * ret=(char *)malloc(msize*sizeof(char));
					int sno=0,ano=0;
					sno=retseq(ackpk);
					ano=retack(ackpk);
					
				}
				
				ackno=sno;
				retmsg(ackpk,ret);
				printf("s-%d a-%d %s\n",sno,ano,ret);
				
				
				addseq(buffer,f1);
				
				i++;
				clock_t now=clock();
				clock_t difference = now - before;
				msec = difference * 1000 / CLOCKS_PER_SEC;
				ttl+=msec;
				if(ttl>=100){
					double tm=tm+(ttl/100),no=(i-j)*500/(1024);
					ttl=0;
					i=0;j=0;
					printf("\n--time %lf size%lf--\n", tm, no);
					fprintf(temp, "%lf %lf \n", tm, no);
					before=now;
				}
				
				
				
			}
			for (i=0; i < 2; i++)
   	 		{
    				fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]);
    			}
    			
			send(connection_fd,"$--End--$",BSIZE,0);
			
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
