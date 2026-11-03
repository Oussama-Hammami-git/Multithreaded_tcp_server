#ifndef MYQUEUE_H_
#define MYQUEUE_H_

#include <stdlib.h>




typedef struct node ts_node;

struct node {
    ts_node* next; 
    int* client_socket;
};

void enqueue(int* client_socket);
int* dequeue();

#endif