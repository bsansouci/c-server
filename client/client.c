#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "llist.h"
#include "client.h"

int init(char* addr, int port){
	printf("Client: Initializing sockfd\n");
	int sockid = socket(AF_INET, SOCK_STREAM, 0);
	if (sockid < 0){
		printf("Client: ERROR sockfd<0\n");
		return -1;
	}
	printf("Client: Done initializing\n");
	
	struct hostent *server = gethostbyname(addr);
	if (server == NULL){
		server = gethostbyaddr(addr, 4, AF_INET);
		printf("Client: the server is null\n");
		if(server == NULL) return -1;
	}
  
  struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	printf("Client: Trying to connect...\n");
	if (connect(sockid,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		printf("Client: Connection Failed\n");
        	return -1;
	}

  return sockid;
}

void sclose(int sock){	
	//shutdown(sock, 2); //close the transmission and reception for this socket
	close(sock);
}


// Those two functions are flawed because they don't take into account the
// fact that the message might be bigger than the packet size.
void sread(int sock, char *message, int message_size) {
	recv(sock, message, message_size, 0);
}

void swrite(int sock, char *message, int message_size) {
	send(sock, message, message_size, 0);
}

char *query(char *addr, int port, char *buffer, int buffer_size, int receive_size) {
	char *return_value = (char *)malloc(receive_size);
	int sock = init(addr, port);
	swrite(sock, buffer, buffer_size);
	sread(sock, return_value, receive_size);
	sclose(sock);
	return return_value;
}
