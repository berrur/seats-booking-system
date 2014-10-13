#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define BUFFER 1024
#define RES_DIM 20

int socket_descriptor;


void show_seatsmap() {
	
	int raws,clmns;	
	char temp[32];
	char * endptr;
	char * check = "CHECK_OK";
	char * mbuffer;

		
	read(socket_descriptor,temp,30);
	printf("%s",temp);
	
	memset(temp,0,32);
	read(socket_descriptor,temp,30);
	raws = strtol(temp,&endptr,10);
	
	write(socket_descriptor,check,strlen(check));
	
	memset(temp,0,32);
	read(socket_descriptor,temp,30);
	clmns = strtol(temp,&endptr,10);

	write(socket_descriptor,check,strlen(check));
	printf("%d e %d\n",raws,clmns);

	int size(raws*clmns + 1024);
	mbuffer = malloc(size);
	memset(mbuffer,0,size);

	read(socket_descriptor,mbuffer,size);
	
	
}

void action_chosen(char * option) {
	if (strcmp(option,"-S\n")==0) {
		show_seatsmap();	
	}
	if (strcmp(option,"-R\n")==0) {
		//reservation
	}
	if (strcmp(option,"-D\n")==0) {
		//deleting reservation
	}
	if (strcmp(option,"-E\n")==0) {
		//exit procedure
	}
}

int connect_function() {
	
	int ds_sock;
	int length_addr;	
	int port = 4444;
	char * ip = "127.0.0.1";
	char msg[BUFFER];
	char action[BUFFER];
	char action_resp[RES_DIM];

	struct sockaddr_in addr;
	
	ds_sock = socket(AF_INET,SOCK_STREAM,0);
	socket_descriptor = ds_sock;	
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ip,&addr.sin_addr);

	length_addr = sizeof(addr);
	if(connect(ds_sock,(struct sockaddr *)&addr,length_addr)==-1) {perror("Connection Error"); exit(1); }
	printf(">>Connected!\n");
	int res;
	if(read(ds_sock,msg,BUFFER)==-1){ perror("Read Error"); }
	printf("=========================================\n");
	printf("%s",msg);
	printf("=========================================\n");
	
	do {

		fgets(action,BUFFER,stdin);
		if(write(ds_sock,action,RES_DIM)==-1){perror("Option selecting error");exit(1);}
		if(read(ds_sock,action_resp,RES_DIM)==-1){perror("Response reading error");exit(1);}
		printf("Action Response: %s\n",action_resp);

	} while(strcmp(action_resp,"RESPONSE_OK") != 0);
	action_chosen(action);	
}

int main() {
	connect_function();
}

