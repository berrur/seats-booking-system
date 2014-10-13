#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#define BACKLOG 10
#define BUFFER 1024
#define RES_DIM 20

/*
 * exit(1) methods have to be replaced with a closing procedure;
 *
 */

struct server_data {
	int raws;
	int clmn;
};

struct server_data info;

struct incoming {
	int socket_descriptor;
	struct sockaddr_in client_data;
};

void * init_connection(void * sender_info) {

	struct incoming * sender = (struct incoming *)sender_info;
	int flag = 0;
	
	char * action_response_ok = "RESPONSE_OK";
	char * action_response_error = "RESPONSE_ERROR";
	char display_options[BUFFER] = "What do you want to do?\n -S show the seats map.\n -R reserve one or more seats.\n -D Cancel reservation.\n -E terminate the session.\n";
	
char option_select[BUFFER];

	if(write(sender->socket_descriptor,display_options,BUFFER)==-1){perror("Writing error, display_option");}
	if(read(sender->socket_descriptor,option_select,BUFFER)==-1){perror("Reading error,option_select");}
	
	do {

		if(strcmp(option_select,"-E\n") == 0) {
			flag = 1;
			write(sender->socket_descriptor,action_response_ok,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		else if(strcmp(option_select,"-S\n") == 0) {
			flag = 1;	
			write(sender->socket_descriptor,action_response_ok,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		else if(strcmp(option_select,"-R\n") == 0) {
			flag = 1;	
			write(sender->socket_descriptor,action_response_ok,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		else if(strcmp(option_select,"-D\n") == 0) {			
			flag = 1;				
			write(sender->socket_descriptor,action_response_ok,RES_DIM);
			perform_action(sender->socket_descriptor,option_select);
		}
		else {
			write(sender->socket_descriptor,action_response_error,RES_DIM);
			read(sender->socket_descriptor,option_select,BUFFER);
		}

	} while(flag != 1);
}

void show_seatsmap(int sd) {
	
	char mat_raws[3];
	char mat_clmns[3];
	char * mbuffer;

	char response[16];	
	memset(response,0,16);
			
	write(sd,"Sending seats map\n",30);
	int fd = open("./seats_map/seats.map",O_RDONLY,0660);
	
	read(fd,mat_raws,3);
	read(fd,mat_clmns,3);

	size_t size = (info.raws*info.clmn) + info.raws;
	
	//Map loading
	mbuffer = (char *)malloc(size);
	if (mbuffer == NULL ) { perror("Malloc Error!"); exit(1); }
	memset(mbuffer,0,size);
	if(read(fd,mbuffer,size) != size) { perror("Something wrong with the read"); }

	write(sd,mat_raws,3);
	
	//Handshake before sending map
	read(sd,response,15);
	printf("%s\n",response);	
	write(sd,mat_clmns,3);
	read(sd,response,15);
	printf("%s\n",response);	
	
	//Sending map
	write(sd,mbuffer,size);
	
	free(mbuffer);	
	close(fd);
}


int listening_function() {
	int ds_sock;
	int port = 4444;
	int length_inc;
	int ds_acc;
	
	struct sockaddr_in addr,inc;
	struct incoming conn_data;

	ds_sock = socket(AF_INET,SOCK_STREAM,0);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
		
	int length_addr = sizeof(addr);

	if(bind(ds_sock,(struct sockaddr *)&addr,length_addr)==-1) { perror("Binding error"); exit(1); }
	if(listen(ds_sock,BACKLOG)==-1) { perror("Listening error"); exit(1); }
	length_inc = sizeof(inc);
	
	while(1) {		
		while((ds_acc = accept(ds_sock,(struct sockaddr *)&inc, &length_inc))==-1 );
			printf(">>Connected to socket %d \n",ds_acc);
			conn_data.client_data = inc;
			conn_data.socket_descriptor = ds_acc;
			init_connection((void *)&conn_data);
	}
}
	

int perform_action(int sock_descriptor, char * option) {
	if (strcmp(option,"-S\n")==0) {
		show_seatsmap(sock_descriptor);
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

int create_map(char * raws, char * columns) {

	int i,j,fd;
	char * endptr;

	info.raws = strtol(raws,&endptr,10);	
	info.clmn = strtol(columns,&endptr,10);	

	if (open("./seats_map/seats.map",O_RDWR,0660) != -1) { return; }	
	else {
		fd = open("./seats_map/seats.map",O_CREAT | O_RDWR,0660);
		if (fd < 0 ) { perror("Error in 'open'"); exit(1); }
	
		write(fd,raws,strlen(raws));
		write(fd,"\n",1);
		write(fd,columns,strlen(columns));
		write(fd,"\n",1);	
	
		for(i=0; i < info.raws; i++) {
			for(j=0; j < info.clmn; j++) {
				write(fd,"X",1);
			}
			write(fd,"\n",1);
		}

		close(fd);
		return 1;
	}	
}

int main() {

	/*	
	 * i'm going to implement this laters
	 *

	sigset_t set;
	if(sigfillset(&set)){ perror("filling set of signals"); exit(-1);}
	struct sigaction sig_act;
	sig_act.sa_handler = close_routine;
	sig_act.sa_mask = set;
	
	if(sigaction(SIGINT,&sig_act,NULL)){ perror("sigaction"); exit(-1);}
	if(sigaction(SIGTERM,&sig_act,NULL)){ perror("sigaction"); exit(-1);}
	if(sigaction(SIGABRT,&sig_act,NULL)){ perror("sigaction"); exit(-1);}
	if(sigaction(SIGHUP,&sig_act,NULL)){ perror("sigaction"); exit(-1);}
	if(sigaction(SIGQUIT,&sig_act,NULL)){ perror("sigaction"); exit(-1);}
	if(sigaction(SIGILL,&sig_act,NULL)){ perror("sigaction"); exit(-1);}
	*/

	create_map("10","20");
	listening_function();

}
