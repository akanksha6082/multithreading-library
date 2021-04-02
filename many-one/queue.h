#ifndef __QUEUE_H
#define __QUEUE_H

#include "athread_types.h"

typedef struct node {
    athread * tcb;
    struct node * next;

}node;


typedef struct queue{
    struct node * front;
    struct node * rear;
    int qcount;
}queue;

int qinit(queue *);
node * create_node(athread *);
void enqueue(queue *, athread *);
athread * dequeue(queue *);
void display(queue *);
athread * search_tcb(queue *, athread_t);
int is_empty(queue *);
int return_count(queue *);


#endif

