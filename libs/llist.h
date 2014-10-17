#ifndef __LLIST_H__
#define __LLIST_H__

struct LList;

struct LList *new_list();

struct Iterator;

struct Iterator *set_iterator(struct LList *list);

void *get_value(struct Iterator *it);

void *get_next(struct Iterator *iterator);

int has_next(struct Iterator *p);

void add_element(void *value, struct LList *list);

void add_element_left(void *value, struct LList *list);

void insert_element(void *value, int index, struct LList *list);

void *get_element(int index, struct LList *list);

void remove_element(int index, struct LList *list);

int get_length(struct LList *list);

#endif
