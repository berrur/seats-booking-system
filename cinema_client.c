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

struct seat{
	unsigned int row;
	unsigned int col;
};

/*
*
* functions declarations
*
*/

void action_chooser();
void show_seatsmap(char * option);
void print_map(char * mbuffer, unsigned int cinema_raws, unsigned int cinema_clmn);
void seats_reservation(char * option);
int connect_function();
//int check_constrains(unsigned int a, unsigned int b);

/*
*	
*	Receive the seats-map from the server and
*	saves it into a matrix,
*
*/

//Check if the seats the client selected previously have doubles
int no_double_seats(struct seat * seats,unsigned int seats_num) {
	int i,j;
	for ( i = 0; i < seats_num; i++ ) {		
		for ( j = 0; j < seats_num; j++ ) {
			if (i == j ) {}
			else if (seats[j].row == seats[i].row && seats[j].col == seats[i].col ) {
				return -1;
			}
		}
	}
	return 1;
}

int check_constrains(unsigned int a,unsigned int b,unsigned int s_num, struct seat * arr) {
	struct seat * punt = arr;
	while( punt - arr < s_num ) {
		if ( punt->row >= a || punt->col >= b ) {
			return 0;	
		}
		punt++;	
	}
	return 1;
}
	
void seats_reservation(char * option) {
	int res;
	char line[100];
	char res_key[11];
	unsigned int seats_num,r,c;

	//loop until sscanf return 0;	
	do{
		printf("Insert the number of seats you want to reserve: \n");
		fgets(line,100,stdin);
		res = sscanf(line,"%u\n",&seats_num); 
	} while(res < 1);
	
	if (seats_num == 0) exit(0);

	
	struct seat seats[seats_num];
	
	int i = 0;

	//loop until sscanf returns one or less value
	while (i < seats_num) {
		do {
				printf("Insert rows and columns for seats[%d]: ",i);
				fflush(stdout);
				fgets(line,100,stdin);
				res = sscanf(line,"%u %u",&seats[i].row,&seats[i].col);
		} while(res<2);
		i++;
	}
	//useful check, high cost
	if (no_double_seats(seats,seats_num) == -1 ) { printf("You choose a seat twice!\n"); exit(1); }
	
	connect_function();		
	write(socket_descriptor,option,10);
	
	//receive cinema sizes
	read(socket_descriptor,&r,sizeof(unsigned int));
	read(socket_descriptor,&c,sizeof(unsigned int));
	printf("%d e %d\n",r,c);
	
	//check bounds
	if (check_constrains(r,c,seats_num,seats)==-1) { printf("Index out of bound!\n"); close(socket_descriptor); exit(1); }
	
	//send the number of seats you want to book
	res = write(socket_descriptor,&seats_num,sizeof(seats_num));
	if(res == -1){ perror("reservation send error"); exit(-1); }
	
	//sends the coordinates of the seats you have chosen
	res = write(socket_descriptor,seats,sizeof(seats));
	if(res < sizeof(seats)) { perror("reservation send error, seats data"); exit(-1); }
	
	if(read(socket_descriptor,line,10) == -1 ) { perror("error reading response"); }
	
	if (strcmp(line,"RES_OK") == 0) {
	
		if( read(socket_descriptor,line,11) == -1 ) { perror("error reading reservation key"); }
		printf(" -----------------------------------------------------\n");			
		printf("|Your reservation code is %s, don't forget it |\n",line);
		printf(" -----------------------------------------------------\n");
		
		close(socket_descriptor);
		exit(1);

	} else {	

		printf("---------------------------------------\n");
		printf("|A seat you choose is already reserved|\n");		
		printf("---------------------------------------\n");		
		close(socket_descriptor);
		exit(1);

	}
}

void show_seatsmap(char * option) {
	int i,j;
	
	unsigned int cinema_raws;
	unsigned int cinema_clmn;
	char temp_read[1];
	char temp[10];
	char * endptr;
	char * mbuffer;
	
	connect_function();
	write(socket_descriptor,option,10);

	read(socket_descriptor,temp,3);
	cinema_raws = strtol(temp,&endptr,10);
		
	memset(temp,0,10);

	read(socket_descriptor,temp,3);
	cinema_clmn = strtol(temp,&endptr,10);

	size_t size = (cinema_raws*cinema_clmn);
	mbuffer = (char *)malloc(size*sizeof(char));
	memset(mbuffer,0,size*sizeof(char));
		
	char (*matrix)[cinema_clmn] = (char (*) [cinema_clmn])mbuffer;		
	for(i = 0; i < cinema_raws; i++) {
		for(j = 0; j < cinema_clmn; j++) {
			read(socket_descriptor,temp_read,1);
			sscanf(temp_read," %c",&matrix[i][j]);
		}
	}

	printf("---------------------------------\n");
	printf("There are overall %d seats\n",(int)size);
	printf("---------------------------------\n");
	print_map(mbuffer,cinema_raws,cinema_clmn);	
	printf("---------------------------------\n");
	close(socket_descriptor);
	exit(1);
}

void delete_reservation(char * option) {
	char key[11];	
	printf("Insert your reservation code\n");
	fgets(key,11,stdin);
	key[11] = '\0';
	
	connect_function();
	write(socket_descriptor,option,10);

	if(write(socket_descriptor,key,20) == -1) { perror("Write error in delete_reservation"); }
	if(read(socket_descriptor,key,20) == -1) { perror("Read error in delete_reservation"); }
	if(strcmp(key,"DEL_CONFIRMED") == 0 ) {
		printf("Your reservation has been deleted succesfully!\n");
	} else {
		printf("Something has gone wrong, please try again!\n");
	}
	close(socket_descriptor);
	exit(1);
}

void print_map(char * mbuffer,unsigned int cinema_raws,unsigned int cinema_clmn) {

	int i,j;
	
	char (*matrix)[cinema_clmn] = (char (*) [cinema_clmn])mbuffer;
	
	for(i = 0; i < cinema_raws; i++) {
		for(j = 0; j < cinema_clmn;j++) {	
			printf("[%d - %d : %c]",i,j,matrix[i][j]);		
 		}
		printf("\n");
	}

}

void action_chooser() {
	char option[10];

	printf("=========================================\n");
	printf("What to do?\n\n");
	printf("-S show the seats map.\n");
	printf("-R reserve one or more seats.\n");
	printf("-D cancel reservation.\n");
	printf("-E terminate the session.\n");
	printf("=========================================\n");
	
	fgets(option,10,stdin);

	do {
	
		if (strcmp(option,"-S\n")==0) {
			show_seatsmap(option);
		}
		else if (strcmp(option,"-R\n")==0) {
			seats_reservation(option);
		}
		else if (strcmp(option,"-D\n")==0) {
			delete_reservation(option);		
		}
		else if (strcmp(option,"-E\n")==0) {
			exit(1);
		}
		else {
			printf("Error: use -R -S -D -E\n");	
		}
		fgets(option,10,stdin);

	} while(1);
}

int connect_function() {
	
	int ds_sock;
	int length_addr;	
	int port = 4444;
	char * ip = "127.0.0.1";

	struct sockaddr_in addr;
	
	socket_descriptor = socket(AF_INET,SOCK_STREAM,0);	
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ip,&addr.sin_addr);	

	length_addr = sizeof(addr);
	if(connect(socket_descriptor,(struct sockaddr *)&addr,length_addr)==-1) { perror("Connection Error"); exit(1); }
	printf("--Estabilished connection with the server--\n");
}

int main() {
	//connect_function();
	action_chooser();
}

