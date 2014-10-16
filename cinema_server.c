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

//reservation_code length is 10 by default

struct server_data {
	int raws;
	int clmn;
	char * matrix;
};
/*
struct incoming {
	int socket_descriptor;
	struct sockaddr_in client_data;
}
*/	
struct reservation { 				//entry of an array of reservations
	char * reservation_code;		//unique code generated for a single reservation
	unsigned int s_num;				//number of seats reserved for a single reservation
	struct seat * seats; 			//array of reserved seats;	
};
struct seat {
	unsigned int row;
	unsigned int col;
};

struct server_data info;
struct reservation * res_list;


void print_matrix();
void occupy_seats(unsigned int s_num,struct seat * seats_occ);
void perform_reservation(unsigned int seats_num,struct seat * seats_occ);

void reservation(int sd) {
	
	int res;
	unsigned int seats_num = 0;
	
	res = read(sd,&seats_num,sizeof(seats_num));
	
	if(res < sizeof(seats_num)){

		if(res == -1)perror("receive number of seats");
		else puts("Error: received invalid seats num");

	}
		
	//receive seats
	struct seat seats[seats_num];
	res = read(sd,seats,sizeof(seats),0);

	if(res < sizeof(seats)) {

		if(res == -1)perror("receive seats");
		else puts("Error: mismatch of seats number recived");
	
	}
	
	occupy_seats(seats_num,seats);
	perform_reservation(seats_num,seats);
	save_reservation_array(info.raws*info.clmn,10);
	
	
}

void perform_reservation(unsigned int seats_num,struct seat * seats_occ) {
	
	int i;
	struct reservation new_entry;
	
	for(i = 0; i < info.raws*info.clmn; i++) {
		if (res_list[i].s_num == 0) { 
			break;
		}
		i++;
	}
	
	new_entry.s_num = seats_num;
	
	void * seats_temp = calloc(seats_num,sizeof(struct seat));
	if (seats_temp == NULL ) { perror("Calloc error in perform reservation!"); }
		
	new_entry.seats = memcpy(seats_temp,seats_occ,sizeof(struct seat)*seats_num);
	new_entry.reservation_code = "codice1234";
	
	res_list[i] = new_entry;
		
}


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
		reservation(sock_descriptor);
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


int save_reservation_array(unsigned int arr_dim,unsigned int chiav_dim){
	
	int res;
	int des_f = open("./seats_res/reservations",O_CREAT | O_WRONLY,0660);
	
	struct reservation * punt = res_list;
	while(punt - res_list < arr_dim){
		
		//write s_num
		res = write(des_f,&(punt->s_num),sizeof(punt->s_num));
		if(res < sizeof(punt->s_num)){
			if(res == -1)
				perror("writing s_num on file");
			else
				puts("error: writing s_num on file");
			return(-1);
		}
		
		if(punt->s_num != 0){
				
			//write resevation_code
			res = write(des_f,punt->reservation_code,chiav_dim+1);
			if(res < chiav_dim+1){
				if(res == -1)
					perror("writing chiavazione on file");
				else
					puts("error: writing chiavazione on file");
			return(-1);
			}
		
			//write the seats array
			res = write(des_f,punt->seats,(punt->s_num)*(sizeof(struct seat)));
			if(res < (punt->s_num)*(sizeof(struct seat))){
				if(res == -1)
					perror("writing seats on file");
				else
					puts("error: writing seats on file");
				return(-1);
			}
		}
		
		punt++;
	}
	return 0;
}

//gotta check if the control s_num != 0 gives error or not


int load_reservation_array(unsigned int arr_dim, unsigned int chiav_dim){
	
	int res;
	int des_f = open("./seats_res/reservations",O_CREAT | O_RDONLY,0660);
	
	struct reservation * punt = res_list;
	while(punt - res_list < arr_dim){
		
		//read s_num
		res = read(des_f,&(punt->s_num),sizeof(punt->s_num));
		if(res < sizeof(punt->s_num)){
			if(res == -1)
				perror("reading s_num from file");
			else
				puts("error: reading s_num from file");
		return(-1);
		}
		
		if(punt->s_num != 0){
			
			punt->reservation_code = malloc(chiav_dim+1);
			if(punt->reservation_code == NULL){perror("error in malloc load_reservation_array");return(-1);}
						
			res = read(des_f,punt->reservation_code,chiav_dim+1);
			if(res < chiav_dim+1){
				if(res == -1)
					perror("reading chiavazione from file");
				else
					puts("error: reading chiavazione from file");
			return(-1);
			}
		
			//read seats arr
			punt->seats = malloc((punt->s_num)*(sizeof(struct seat)));
			if(punt->seats == NULL){perror("error in malloc load_reservation_array");return(-1);}
			
			res = read(des_f,punt->seats,(punt->s_num)*(sizeof(struct seat)));
			if(res < (punt->s_num)*(sizeof(struct seat))){
				if(res == -1)
					perror("reading seats from file");
				else
					puts("error: reading seats from file");
				return(-1);
			}
			
			//refill matrix
			occupy_seats(punt->s_num,punt->seats);
		}
		punt++;	
	}
	return 0;
}

void occupy_seats(unsigned int num, struct seat * seats_occ) {
	char (*matrix)[info.clmn] =(char (*)[info.clmn]) info.matrix;
	struct seat * punt = seats_occ;
	while( (punt - seats_occ) < num ) {

		//if(matrix[punt->row][punt->col] == 1) { puts("found seats already occupied"); exit(-1); }
		matrix[punt->row][punt->col] = 'O';
		punt++;

	}
}

void res_list_init() {

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
	struct reservation posti_occupati[info.raws*info.clmn*sizeof(struct reservation *)];
	res_list = posti_occupati;
	matrix_init();
	load_reservation_array(info.raws*info.clmn,10);
	listening_function();

}
