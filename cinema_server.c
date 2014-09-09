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

struct incoming {
	int socket_descriptor;
	struct sockaddr_in client_data;
};

void * thread_response(void * sender_info) {
	char msg[1024];
	char * display_options;
	struct incoming * sender = (struct incoming *)sender_info;
	int ds = sender->socket_descriptor;
	display_options = "What do you want to do?\n -S show the seats map.\n -R reserve one or more seats.\n -E terminate the session.\n";
	printf("Descrittore socket sul thread:%d\n",ds);	
	int len = strlen(display_options);
	if (write(sender->socket_descriptor,&display_options,len) == -1 ) {
		perror("Writing error, display_option");
	}	printf("fatto");
	if (read(sender->socket_descriptor,msg,BUFFER) > 0) {	
		printf("SENDER:%s",msg);fflush(stdout);
	} //tanti problemi qua, ma taaaaanti
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
			printf(">>Connected to socket %d",ds_acc);
			conn_data.client_data = inc;
			conn_data.socket_descriptor = ds_acc;
			if(pthread_create(&tid,NULL,*thread_response,(void *)&conn_data)!= 0) {	perror("Thread Creation error");	}
	}	
}

int main() {
	listening_function();
}
