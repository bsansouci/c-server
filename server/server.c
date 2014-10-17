#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include "llist.h"
#include "server.h"

// Function declarations
int init(void);
void* listener();
extern int parse_members(void);
extern int parse_inventory(void);
extern struct LList *parse(char *source, char* deliminator);
void* event_manager();
void* socket_thread();

//global variables
int running = 1;
struct LList *event_queue;
struct LList *query_list;
pthread_t event_thread, listen_thread;

// Definition of an Event
// When receiving a query, the listen_thread
struct Event {
	int socket;
	char *request;
};

// Definition of query-able function
// query is a char* representing the name of the function
// func is a function pointer that is going to be called when a query is
// received and treated by the event_thread
struct Query {
	char *query;
	char *(*func)();
};

/**
 *  This function will instantiate an event_queue and a query list.
 *
 *  The listen_thread will push events on the event_queue, and the event_thread
 *  will pop events and deal with them one at a time.
 *
 *  The query_list is going to be used by to store Query structs, and execute
 *  the query that was received from the client. The query sent by the client
 *  only contains the name of a function to be called on the server side, which
 *  will return a value that will be sent back to the client.
 *
 * @return Error code
 */

int init(void) {
	event_queue = new_list();
	query_list = new_list();
  
  // Create the event_thread and the listen_thread
	int iret = pthread_create( &event_thread, NULL, event_manager, (char *) "1");
  int iret2 = pthread_create( &listen_thread, NULL, listener, (char *) "2");
  
  // We start by thinking that there will be no error. But if there are, we
  // simply set running to false and return
	if(iret + iret2 != 0) running = 0;

	return iret + iret2; // Return error code
}

/**
 *  Function that will join event_thread and listen_thread. This is to prevent
 *  the main thread from quitting before the server receives a "quit" command.
 */
void server_wait(void) {
  pthread_join(event_thread, NULL);
  pthread_join(listen_thread, NULL);
  
  // Once the threads are done we stop this thread and do some cleanup if
  // necessary
  pthread_exit(NULL);
}

/**
 *  Listener function that will be running in the listen_thread. This function
 *  will be looping, listening for incoming connections. Once it receives a
 *  request to connect it will connect with the client and create a thread to
 *  deal with receiving the packet from that client.
 * 
 *  @param id id of the thread
 *
 *  @return unused
 */
void* listener(void* id) {
	//initialization
	int portno = 5037;
	struct sockaddr_in serv_addr, cli_addr;

  // Open a socket through which we'll be able to listen for clients
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
	if(sockfd < 0)
        printf("Server: ERROR opening the socket\n");
  
  // Set to zero
//  memset((char *) serv_addr,'\0', sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);

	printf("Server: Initializing done!\n");

	//binding socket
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		printf("Server: ERROR on binding");
	
	printf("Server: Binding done!\n");
	
	//main loop listening
	while(running) {
		// Pre-allocate things so that we can deal with an incoming connection
    // as quickly as possible
    socklen_t clilen = sizeof(cli_addr);
    
    // This will pause the execution of the loop until there's a connection
		listen(sockfd, 1);
		// We accept the connection (accept returns an int)
		int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
    if(newsockfd < 0) printf("Server: ERROR on accept");
        
		printf("Server: New Socket accepted\n");
		
    int copy = newsockfd;
    pthread_t sock_thread;
		int iret = pthread_create(&sock_thread, NULL, socket_thread, &copy);
    
    if(iret != 0) printf("Error when creating socket_thread");
	}
	printf("Server: Closing sockets...\n");

  //closing sockets
	close(sockfd);
  pthread_exit(NULL);
}

/**
 *  Function to take care of an incoming connection with a socket.
 *
 *  @param sock socket id which is used in the recv function to receive the 
 *         packets.
 *
 *  @return unused
 */
void* socket_thread(void* sock) {
  // A little bit ugly, but we know it's a pointer so we copy in a local
  // variable. This is to avoid problems if the listen_thread receives another
  // incoming connection before this thread has time to use *sock, which will
  // change as soon as there's a new connection.
  int thread_id = *(int *)sock;
  
  // This is pretty bad practice too because it assumes the message won't be
  // longer than 1000 characters. A better way to do this would be to send a
  // first packet with an agreed upon size, containing the size of the
  // upcoming packets of data
	int buffer_size = 1000;
	
  // Initialize the buffer and set it to 0
  char *buffer = (char *) malloc(sizeof(char) * buffer_size);
  memset((char *)buffer,'\0', sizeof(char) * buffer_size);
  
  // This while loop is probably going to run only once. So if the client sends
  // bigger messages, the server will only get a truncated version.
  // The client's first packet should be an integer to define the size of the
  // whole packet.
	while(strlen(buffer) < 1){
		recv(thread_id, buffer, buffer_size, 0);
	}
	struct Event *new_event = (struct Event *)malloc(sizeof(struct Event));
	new_event->socket = *(int *)sock;
	new_event->request = (char *)malloc(strlen(buffer)+1);
	strcpy(new_event->request, buffer);
    
  // We're done with buffer, we can free the memory
  free(buffer);
	
  add_element(new_event, event_queue);
  pthread_exit(NULL);
}

/**
 *  This function will loop and check if the event_queue contains anything.
 *  If it does, this function will take care of the query.
 *
 *  @param id id of the thread
 *
 *  @return <#return value description#>
 */
void* event_manager(char* id) {
	while(running) {
    // This is to avoid clogging up the CPU, but it's definitely not a viable solution
    // A better way to do that is to have the thread pause until there's a new element in the list
    // That said, if there's only ever going to be one client sending requests, then this solution might perform better due to overhead of
    // putting a thread to sleep or waking it up.
		usleep(100);
    // If the event_queue contains anything
		if(get_length(event_queue) > 0) {
      // We get the first element
			struct Event *event = (struct Event *)get_element(0, event_queue);
			
      char* request = event->request;
			int socket = event->socket;
      
      // We parse the query such that the first word will be the name of the
      // function. The whole list will be passed as arguments to the function
			struct LList *list = parse(request, " ");
			printf("Request: %s\n", request);
      
      // We now loop through our list of functions and find the one with the
      // same name as the first element of the list list.
			for(int i = 0; i < get_length(query_list); i++) {
				struct Query *query = (struct Query *)get_element(i, query_list);
				
				if(strcmp(query->query, (char *)get_element(0, list)) == 0) {
          // We then call the function with the right name, passing as arguments
          // the list containing the name of the function and any other data
          // that we want.
					char* response = (((struct Query *)get_element(i, query_list))->func)(list);
					printf("Found answer: %s\n", response);
          
          // We then send the response back to the client.
					send(socket, response, strlen(response), 0);
				}
			}
			
			shutdown(socket, 2); //close the transmission and reception for this socket
			remove_element(0, event_queue);
		}
	}
  pthread_exit(NULL);
}

/**
 *  This function will add functions as possible queries.
 *
 *  @param query name of a function
 *  @param func  function pointer to be called when a query is received with the
 *               name of this function
 */
void add_function(char *query, char *(*func)()) {
	struct Query *new_query = (struct Query *)malloc(sizeof(struct Query));
	new_query->query = query;
	new_query->func = func;
	add_element(new_query, query_list);
	printf("Added function: %s\n", ((struct Query *)get_element(0, query_list))->query);
}
