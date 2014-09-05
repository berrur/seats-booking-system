#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

struct incoming {
	int socket_descriptor;
	struct sockaddr_in client_data;
};

int function() {
	int ds_sock;
	int port = 4444;
	
	struct sockaddr_in addr,inc;
	
	//definizione socket
	ds_sock = socket(AF_INET,SOCK_STREAM,0);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton("127.0.0.1",&addr.sin_addr);
	
	//fase di binding e listening
	int length_addr = sizeof(addr);
	if ( bind(ds_sock,(struct sockaddr *)&addr, length_addr) == -1 ) {
		perror("Binding error");
		exit(1);	
	}

	if ( listen(ds_sock,BACKLOG) == -1 ) {
		perror("Listening error");
		exit(1);
	}

	while(1) {
		length_inc = sizeof(inc);
		while( (ds_acc = accept(ds_sock,(struct sockaddr *)&inc, &length_inc) ) == -1 ) {
			printf("CONNECTED: socket %d",ds_acc);
			conn_data.client_data = inc;
			conn_data.socket_descriptor = ds_acc;
		}
	}	
}

int main() {
	function();
}
