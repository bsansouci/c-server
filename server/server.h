#ifndef __SERVER_H__
#define __SERVER_H__
int init(void);
void add_function(char *query, char *(*func)());
void server_wait();
#endif
