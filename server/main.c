//
//  main.c
//  server
//
//  Created by Benjamin San Soucie on 10/16/14.
//  Copyright (c) 2014 personal. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "llist.h"
#include "server.h"

char* test(struct LList* list) {
	return (char *)get_element(1, list);
}

char* test2(struct LList* list) {
	if (strcmp((char *)get_element(1, list), "helloworld") == 0) {
		return "this is exactly what I wanted to hear";
	}
  
	return "I don't understand";
}

int main() {
	init();
	add_function("test", &test);
	add_function("test2", &test2);
    
	// We need to wait because the main thread will quit and stop the event_thread and the listening_thread
	server_wait();
	return 1;
}