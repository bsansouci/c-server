#include <stdlib.h>
#include <string.h>
#include "llist.h"

struct Node{
	void *value;
	struct Node *next;
	struct Node *prev;
};

struct LList{
	struct Node *head;
	struct Node *tail;
	int length;
};

struct LList *new_list(){
	struct LList *list = (struct LList*)malloc(sizeof(struct LList));
	list->head = NULL;
	list->tail = NULL;
	list->length =0;
	return list;
}

struct Iterator {
	struct Node *ptr;
};

struct Iterator *set_iterator(struct LList *list) {
	struct Iterator *new_iterator = (struct Iterator *)malloc(sizeof(struct Iterator));
	struct Node *dummy_head = (struct Node *)malloc(sizeof(struct Node));
	dummy_head->next = list->head;
	dummy_head->value = "dummy_head";
	new_iterator->ptr = dummy_head;
	return new_iterator;
}

void *get_value(struct Iterator *it) {
	return it->ptr->value;
}

void *get_next(struct Iterator *iterator) {
	if(iterator->ptr->next != NULL) {
		iterator->ptr = iterator->ptr->next; //not sure if that works or not
		return iterator->ptr->value;
	}
	else
		return NULL;
}

int has_next(struct Iterator *p) {
	if (p->ptr->next == NULL)
		return 0;
	else
		return 1;
}

void add_element(void *value, struct LList *list){
	// Add an element to the right of the list
	struct Node *node = (struct Node *)malloc(sizeof(struct Node));
	struct Node *tmp;
	node->value = value;
	node->next = NULL;
	node->prev = NULL;
	
	if (list->head == NULL){
		list->head = node;
		list->tail = node;
		list->length += 1;
		return;
	}
	node->prev = list->tail;
	tmp = list->tail;
	list->tail = node;
	tmp->next = node;
	list->length += 1;
}

void add_element_left(void *value, struct LList *list){
	// Add an element to the right of the list
	struct Node *node = (struct Node *)malloc(sizeof(struct Node));
	node->value = value;
	node->next = NULL;
	node->prev = NULL;
	
	if (list->head == NULL){
		list->head = node;
		list->tail = node;
		list->length += 1;
		return;
	}
	node->next = list->head;
	list->head->prev = node;
	list->head = node;
	list->length += 1;
}

void insert_element(void *value, int index, struct LList *list){
	if(index < 0 && index > list->length)
		return;
	if(index==0) {
		add_element_left(value, list);
		return;
	}
    
    int i=0;
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    struct Node *cur;
    node->next = NULL;
    node->prev = NULL;
    node->value = value;
	
	if(index < list->length/2) {
		cur = list->head;
		for(i=0; i<index; i++) {
			cur = cur->next;
		}
		node->prev = cur->prev;
		node->next = cur;
		cur->prev->next = node;
		cur->prev = node;
	}
	else {
		cur = list->tail;
		for(i=list->length-1; i > index; i--) {
			cur = cur->prev;
		}
		node->prev = cur->prev;
		node->next = cur;
		(cur->prev)->next = node;
		cur->prev = node;
	}
	list->length += 1;	
}

void *get_element(int index, struct LList *list){
	struct Node *cur;
	int i;
	if (index < 0 || index >= list->length){
		return NULL;
	}
	if (index < list->length/2) {
		cur = list->head;
		for(i=0; i < index; i++){
			cur = cur->next;
		}
		return cur->value;
	}
	else { // added this for efficiency
		cur = list->tail;
		for(i=list->length-1; i > index; i--){
			cur = cur->prev;
		}
		return cur->value;
	}
}

void remove_element(int index, struct LList *list){
	struct Node *cur;
	int i;
	if (index < 0 || index >= list->length){
		return;
	}
	if(index == 0) {
		if(list->length == 1) {
			list->head = NULL;
			list->tail = NULL;
			list->length = 0;
			return;
		}
		else {
			list->head = list->head->next;
			list->head->prev = NULL;
			list->length -= 1;
			return;
		}
	}
	if (index < list->length/2) {
		cur = list->head;
		for(i=0; i < index; i++){
			cur = cur->next;
		}
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
		//free(cur);
		cur = NULL;
		list->length -= 1;
	}
	else { // added this for efficiency
		cur = list->tail;
		for(i=list->length-1; i > index; i--){
			cur = cur->prev;
		}
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
		//free(cur);
		cur = NULL;
		list->length -= 1;
	}
}

int get_length(struct LList *list){
	return list->length;
}
