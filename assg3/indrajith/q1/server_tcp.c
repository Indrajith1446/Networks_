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

#define MAX 80


struct inventory{
	int apple;
	int mango;
	int banana;
	int chickoo;
	int papaya;
};

void in_ret(struct inventory *inv, char *buf){
	sprintf(buf,"Apple %d\nMango %d\nBanana %d\nChickoo %d\nPapaya %d\n",inv->apple,inv->mango,inv->banana,inv->chickoo,inv->papaya);
	
	return ;

}
int is_in_inv(char *str){
	if(!strcmp(str,"apple")&&!strcmp(str,"mango")&&!strcmp(str,"banana")&&!strcmp(str,"chickoo")&&!strcmp(str,"papaya"))
		return 0;
	return 1;
}
int changeval(char *name,int value, struct inventory *inv)
{
	if(strcmp(name,"apple")==0){
		if(inv->apple<value)
			return 0;
		else
			inv->apple-=value;
	}
	else if(strcmp(name,"mango")==0){
		if(inv->mango<value)
			return 0;
		else
			inv->mango-=value;
	}
	else if(strcmp(name,"banana")==0){
		if(inv->banana<value)
			return 0;
		else
			inv->banana-=value;
	}
	else if(strcmp(name,"chickoo")==0){
		if(inv->chickoo<value)
			return 0;
		else
			inv->chickoo-=value;
	}
	else if(strcmp(name,"papaya")==0){
		if(inv->papaya<value)
			return 0;
		else
			inv->papaya-=value;
	}
	else{
		return 0;
	}
}
int isvalid(char *temp, struct inventory *inv)
{
	char *name=(char *)malloc(100*sizeof(char));
	char *val=(char *)malloc(10*sizeof(char));
	char *buff=(char *)malloc(sizeof(temp));
	strcpy(buff,temp);
	int value,n=0;
	printf(" %s",buff);

	while(buff[n]!=' ')
		name[n++]=buff[n];
	name[n+1]='\0';
	int i=0;
	n=strlen(name);
	while(buff[n]!='\n')
		val[i++]=buff[n++];
	val[i+1]='\0';
	//printf("name%s/%s",name,val);
	if(is_in_inv(name)){
		int i=0;

		//printf("value%s-",val);
		if(!sscanf(val,"%d",&value))
			return 0;
		//printf(" %d",value);
		if(changeval(name,value,inv)){
			strcpy(temp,"Value changed");
			return 1;
		}
		else{
			strcpy(temp,"Not available");
			return 1;
		}
		return 0;
	}else{
		return 0;
	}
	return 0;
}


struct inventory *inv;


int fun(int connection_fd){
	char buff[MAX];
	
	for(;;){
		bzero(&buff,MAX);
		read(connection_fd, buff, sizeof(buff));
		
		
		
		if(strncmp(buff, "stop", 4)==0){
			printf("Closing connection by server\n");
			break;
		}
		else if(strcmp(buff, "SendInventory\n")==0){
			bzero(&buff,MAX);
			in_ret(inv, buff);
			
		
			printf("%s",buff);
			write(connection_fd,buff,sizeof(buff));
			continue;
					
		}
		else if(strcmp(buff, "Fruits\n")==0){
			bzero(&buff,MAX);
			strcpy(buff, "Enter the name of the fruit\n");
			write(connection_fd,buff,sizeof(buff));
			while(1){
				
				bzero(&buff, MAX);
				read(connection_fd, buff, sizeof(buff));
				//printf("tst1");
				char *temp;
				temp=(char *)malloc(MAX*sizeof(char));
				strcpy(temp,buff);
				if(isvalid(temp, inv)==1){
					//printf("tst2");
					write(connection_fd,temp,sizeof(buff));
					break;
				}
				bzero(&buff, MAX);
				strcpy(buff, "Invalid format or name(fomat:- fruitname count)\nEnter the name of the fruit/n");
				write(connection_fd,buff,sizeof(buff));
			}
			continue;
			
			
		}
		else{
			printf("\nFrom client: %s",buff);
			bzero(&buff,MAX);
			
			strcpy(buff,"Enter something useful");
		}
		write(connection_fd,buff,sizeof(buff));
		
	}
	
	return 0;
}
void init(struct inventory *inv){
	inv->apple=200;
	inv->mango=500;
	inv->banana=50;
	inv->chickoo=500;
	inv->papaya=5;
}


int main(){
	int socket_fd,connection_fd,len;
	inv=(struct inventory *)malloc(sizeof(struct inventory));
	init(inv);
	
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
