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

void init(struct inventory *inv){
	inv->apple=200;
	inv->mango=500;
	inv->banana=50;
	inv->chickoo=500;
	inv->papaya=5;
}


int main() {
	int socket_fd;
	char buffer[MAX];
	char *hello = "Hello from server";
	
	inv=(struct inventory *)malloc(sizeof(struct inventory));
	init(inv);
	
	struct sockaddr_in server, client;
	
	if ( (socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);
	
	if ( bind(socket_fd, (const struct sockaddr *)&server,
			sizeof(server)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	int len, n;

	len = sizeof(client);
	
	//receive
	n = recvfrom(socket_fd, (char *)buffer, MAX,
				MSG_WAITALL, ( struct sockaddr *) &client,
				&len);
	buffer[n] = '\0';
	printf("Client : %s\n", buffer);
	
	//send
	sendto(socket_fd, (const char *)hello, strlen(hello),
		MSG_CONFIRM, (const struct sockaddr *) &client,
			len);
	printf("Hello message sent.\n");
	
	for(;;){
	
		bzero(&buffer,sizeof(buffer));
		
		//receive
		n = recvfrom(socket_fd, (char *)buffer, MAX,
				MSG_WAITALL, ( struct sockaddr *) &client,
				&len);
		buffer[n] = '\0';
		printf("Client : %s\n", buffer);
		
		if(strncmp(buffer,"stop",4)==0){
			printf("closing connection by server");
			break;
		}
		else if(strcmp(buffer, "SendInventory\n")==0){
			bzero(&buffer,MAX);
			in_ret(inv, buffer);
			
			
			printf("%s",buffer);
			sendto(socket_fd, buffer, strlen(buffer),
				MSG_CONFIRM, (const struct sockaddr *) &client,
				len);
			
			continue;
					
		}
		else if(strcmp(buffer, "Fruits\n")==0){
			bzero(&buffer,MAX);
			strcpy(buffer, "Enter the name of the fruit\n");
			sendto(socket_fd, buffer, strlen(buffer),
				MSG_CONFIRM, (const struct sockaddr *) &client,
				len);
			while(1){
				
				bzero(&buffer, MAX);
				n = recvfrom(socket_fd, (char *)buffer, MAX,
					MSG_WAITALL, ( struct sockaddr *) &client,
					&len);
				buffer[n] = '\0';
				printf("tst1");
				char *temp;
				temp=(char *)malloc(MAX*sizeof(char));
				strcpy(temp,buffer);
				if(isvalid(temp, inv)==1){
					printf("tst2");
					sendto(socket_fd, temp, strlen(temp),
						MSG_CONFIRM, (const struct sockaddr *) &client,
						len);
					break;
				}
				bzero(&buffer, MAX);
				strcpy(buffer, "Invalid format or name(fomat:- fruitname count)\nEnter the name of the fruit/n");
				sendto(socket_fd, buffer, strlen(buffer),
					MSG_CONFIRM, (const struct sockaddr *) &client,
					len);
			}
			continue;
			
			
		}
		else{
			printf("\nFrom client: %s",buffer);
			bzero(&buffer,MAX);
			
			strcpy(buffer,"Enter something useful");
		}
		//send
		sendto(socket_fd, buffer, strlen(buffer),
			MSG_CONFIRM, (const struct sockaddr *) &client,
			len);
		
	}
	
	return 0;
}

