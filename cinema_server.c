#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <argp.h> 


#define BACKLOG 10
#define BUFFER 1024
#define RES_DIM 20

/*
* exit(1) methods have to be replaced with a closing procedure;
*
*/
void print_matrix();

struct server_data {
	int raws;
	int clmn;
	char * matrix;
};
struct incoming {
	int socket_descriptor;
	struct sockaddr_in client_data;
};
struct reservation {
	int number_seats_reserved;
	char * seats_reserved;
	char * reservation_code;
};

struct server_data info;

/*
void init_connection(int sd) {

	char option[10];

	if(read(sd,option,10)==-1) {perror("Reading error init_connection");}
	perform_action(sd,option);
}
*/


void show_seatsmap(int sd) {
	
	char mat_raws[3];
	char mat_clmns[3];
	char option[10];
	char * mbuffer;

	//memset(option,0,10);
	
	sprintf(mat_clmns,"%d",info.clmn);
	sprintf(mat_raws,"%d",info.raws);

	//Handshake before sending map
	write(sd,mat_raws,3);	
	write(sd,mat_clmns,3);
	
	//Sending map;
	char * qua;
	int i,j;	
	char (*temp_matrix)[info.clmn] = (char (*)[info.clmn])info.matrix;
	char str_buff[1];
	
	for(i = 0; i < info.raws; i++) {
		for(j = 0; j < info.clmn; j++) {
			sprintf(str_buff,"%c",temp_matrix[i][j]);
			write(sd,str_buff,1);
		}
	}
	perform_action(sd);

	
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
			perform_action(ds_acc);
	}
}

int perform_action(int sock_descriptor) {
	char option[10];

	if(read(sock_descriptor,option,10)==-1) {perror("Reading error init_connection");}

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

int create_map(int raws,int columns) {

	int i,j,fd;
	char * endptr;

	info.raws = raws;	
	info.clmn = columns;	

	if (open("./seats_map/seats.map",O_RDWR,0660) != -1) { return; }	
	else {
		fd = open("./seats_map/seats.map",O_CREAT | O_RDWR,0660);
		if (fd < 0 ) { perror("Error in 'open'"); exit(1); }
		
		for(i=0; i < info.raws; i++) {
			for(j=0; j < info.clmn; j++) {
				write(fd,"X",1);
			}
		}
	
		close(fd);
		return 1;
	}	
}

void matrix_init() {
	int 	i,j;
	char temp_string[1];
	char temp_char;
	
	int fd = open("./seats_map/seats.map",O_RDONLY,0660);
	if (fd < 0) { perror("Open issue at matrix_init"); exit(1); }

	info.matrix = (char *)malloc(info.raws*info.clmn*sizeof(char));
	char (*temp_matrix)[info.clmn] = (char (*)[info.clmn])info.matrix;	
	//How to read this ^: "temp_matrix is a pointer of info.clmn characters"
	
	for(i = 0; i < info.raws; i++) {
		for(j = 0; j < info.clmn; j++) {	
			read(fd,temp_string,1);
			sscanf(temp_string," %c",&temp_matrix[i][j]);	
		}
	}
	print_matrix();
}

void print_matrix() {
	int i,j;
	char (*temp_matrix)[info.clmn] = (char (*)[info.clmn])info.matrix;	
	
	for(i = 0; i < info.raws; i++) {
		for(j = 0; j < info.clmn; j++) {
			printf("[%c] ",temp_matrix[i][j]);
		}
		printf("\n");
	}
}

error_t parse_opt(int key,char* arg,struct argp_state * state) {
	switch(key) {
		case 'n':
					//dosomething
					printf("ciao\n");
	}
	return 0;
}


int main(int argc, char **argv) {
	
	/*	i'm going to implement this later
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
	
	create_map(4,4);
	matrix_init();
	listening_function();

}
