//
//  main.c
//  client
//
//  Created by Benjamin San Soucie on 10/16/14.
//  Copyright (c) 2014 personal. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

int main(void) {
  char* password_peter = query("localhost", 5037, "test this_is_the_second_element_of_the_list", 1000, 1000);
  printf("Client received 1: %s\n", password_peter);
  free(password_peter);
    
  char* answer = query("localhost", 5037, "test2 helloworld", 1000, 1000);
  printf("Client received 2: %s\n", answer);
  free(answer);
  
  getchar();
  return 0;
}
