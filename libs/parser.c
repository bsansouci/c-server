#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "llist.h"
#include "parser.h"

struct LList *parse(char *source, char *deliminator){
	char buffer[strlen(source)+1];
	unsigned long delim_len = strlen(deliminator);
	struct LList *list = new_list();
	char *tmp;
	char c;
	int i, j, k=0, index;

	for (i=0; i < strlen(source); i++){
		if (source[i] == deliminator[0]){
			index = i;
			for (j=0; source[index+j] == deliminator[j]; j++){
				if (j+1 == delim_len){
					buffer[k] = '\0';
					tmp = malloc(sizeof(char)*strlen(buffer)+1);
					strcpy(tmp, buffer);
					add_element(tmp, list);
					k = 0;
					i += delim_len;
					break;
				}
			}
		}
		c = source[i];
		buffer[k] = c;
		k++;
	}
	buffer[k] = '\0';
	tmp = malloc(sizeof(char)*strlen(buffer)+1);
	strcpy(tmp, buffer);
	add_element(tmp, list);
	return list;
}
