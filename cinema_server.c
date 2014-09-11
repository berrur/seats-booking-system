#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define BACKLOG 10
#define BUFFER 1024
#define RES_DIM 20

/*
 * exit(1) methods have to be replaced with a closing procedure;
 *
 */

struct incoming {
	int socket_descriptor;
	struct sockaddr_in client_data;
};

void * thread_response(void * sender_info) {

	struct incoming * sender = (struct incoming *)sender_info;
	int flag = 0;
	char * action_response;
	char option_select[1024];
	char display_options[BUFFER] = "What do you want to do?\n -S show the seats map.\n -R reserve one or more seats.\n -D Cancel reservation.\n -E terminate the session.\n";

	if(write(sender->socket_descriptor,display_options,BUFFER)==-1){perror("Writing error, display_option");}
	if(read(sender->socket_descriptor,option_select,BUFFER)==-1){perror("Reading error,option_select");}

	do {

		if(strcmp(option_select,"-E\n") == 0) {
			flag = 1;
			action_response = "RESPONSE_OK";			
			write(sender->socket_descriptor,action_response,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		if(strcmp(option_select,"-S\n") == 0) {
			action_response = "RESPONSE_OK";			
			write(sender->socket_descriptor,action_response,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		if(strcmp(option_select,"-R\n") == 0) {
			action_response = "RESPONSE_OK";			
			write(sender->socket_descriptor,action_response,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		if(strcmp(option_select,"-D\n") == 0) {			
			action_response = "RESPONSE_OK";			
			write(sender->socket_descriptor,action_response,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
	
		action_response = "RESPONSE_ERROR";
		write(sender->socket_descriptor,action_response,RES_DIM);
		read(sender->socket_descriptor,option_select,BUFFER);

	} while(flag != 1);
}


int listening_function() {
	int ds_sock;
	int port = 4444;
	int length_inc;
	int ds_acc;

	pthread_t tid;
	
	struct sockaddr_in addr,inc;
	struct incoming conn_data;
	
	////////////////////////////////////////

	ds_sock = socket(AF_INET,SOCK_STREAM,0);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	
	int length_addr = sizeof(addr);
	

	//Binding and Listening phase
	if(bind(ds_sock,(struct sockaddr *)&addr,length_addr)==-1) { perror("Binding error"); exit(1); }
	if(listen(ds_sock,BACKLOG)==-1) { perror("Listening error"); exit(1); }
	
	length_inc = sizeof(inc);
	while(1) {		
		while((ds_acc = accept(ds_sock,(struct sockaddr *)&inc, &length_inc))==-1 );
			printf(">>Connected to socket %d \n",ds_acc);
			conn_data.client_data = inc;
			conn_data.socket_descriptor = ds_acc;
			if(pthread_create(&tid,NULL,*thread_response,(void *)&conn_data)!= 0) {	perror("Thread Creation error"); }
	}
}

int perform_action(int sock_descriptor, char * option) {
	if (strcmp(option,"-S\n")==0) {
		//show the map
		printf("Sto qua? + %s + %d\n",option,sock_descriptor);
		write(sock_descriptor,"mappa!",8);	
	}
}

int main() {
	listening_function();
}
