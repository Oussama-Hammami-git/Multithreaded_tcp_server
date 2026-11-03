#include "my_queue.h"

ts_node* head = NULL;
ts_node* tail = NULL;


void enqueue(int* client_socket)
{
    ts_node* newNode = malloc(sizeof(ts_node));
    newNode->client_socket = client_socket;
    newNode->next = NULL;

    if(tail == NULL)
    {
        head = newNode;
    }
    else
    {
        tail->next = newNode;
    }
    tail = newNode;
}

int* dequeue()
{
    if(head == NULL)
    {
        return NULL;
    }
    else {
        int* result = head->client_socket;
        ts_node* temp = head;
        head = head->next;
        if(head == NULL) tail = NULL;
        free(temp);
        return result;
    }
}




