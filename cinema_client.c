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
int cinema_raws;
int cinema_clmn;

struct seat{
	unsigned int row;
	unsigned int col;
};

/*
*
* functions declarations
*
*/

void action_chooser(int sd);
void show_seatsmap();
void print_map(char * mbuffer);
void seats_reservation();
int connect_function();

/*
*	
*	Receive the seats-map from the server and
*	saves it into a matrix,
*
*/



void seats_reservation() {

	int res;
	char line[100];
	unsigned int seats_num;

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

	res = write(socket_descriptor,&seats_num,sizeof(seats_num));
	if(res == -1){perror("send");exit(-1);}
	
	res = write(socket_descriptor,seats,sizeof(seats));
	if(res < sizeof(seats)) { perror("send seats data"); exit(-1); }


}


void show_seatsmap() {
	int i,j;
	
	char action[4];
	char temp_read[1];
	char temp[10];
	char * endptr;
	char * mbuffer;
	char * check = "CHECK_OK";

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
	print_map(mbuffer);	
	printf("---------------------------------\n");

	printf("Insert -E to terminate this session\n");
	printf("Or insert -R to book a seats\n");
	return;
}

void print_map(char * mbuffer) {

	int i,j;
	
	char (*matrix)[cinema_clmn] = (char (*) [cinema_clmn])mbuffer;
	
	for(i = 0; i < cinema_raws; i++) {
		for(j = 0; j < cinema_clmn;j++) {	
			printf("[%d - %d : %c]",i,j,matrix[i][j]);		
 		}
		printf("\n");
	}

}

void action_chooser(int sd) {
	char option[10];

	printf("=========================================\n");
	printf("What to do?\n");
	printf("-S show the seats map.\n");
	printf("-R reserve one or more seats.\n");
	printf("-D cancel reservation.\n");
	printf("-E terminate the session.\n");
	printf("=========================================\n");
	
	fgets(option,10,stdin);

	printf("%d\n",(int)strlen(option));
	
	do {
	
		if (strcmp(option,"-S\n")==0) {
			write(sd,option,10);
			show_seatsmap();
		}
		else if (strcmp(option,"-R\n")==0) {
			write(sd,option,10);
			seats_reservation();
		}
		else if (strcmp(option,"-D\n")==0) {
			//deleting reservation
		}
		else if (strcmp(option,"-E\n")==0) {
			//exit procedure
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
	char msg[BUFFER];
	char action[BUFFER];
	char action_resp[RES_DIM];

	struct sockaddr_in addr;
	
	socket_descriptor = socket(AF_INET,SOCK_STREAM,0);	
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ip,&addr.sin_addr);

	length_addr = sizeof(addr);
	if(connect(socket_descriptor,(struct sockaddr *)&addr,length_addr)==-1) {perror("Connection Error"); exit(1); }
	printf(">>Connected!\n");
		
}

int main() {
	connect_function();
	action_chooser(socket_descriptor);
}

