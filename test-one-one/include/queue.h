#ifndef __QUEUE_H
#define __QUEUE_H

#include "athread_types.h"

typedef struct node {
    athread * tcb;
    struct node * next;
}node;

typedef struct queue {

    struct node *front, *rear;
    int count;
}queue;

void qinit( queue * );

void enqueue(queue *, athread * );

node * dequeue(queue *);

void display(queue *);

int return_qcount(queue *);

athread * search_tcb(queue *, athread_t);

int is_empty(queue * );

node * create_node( athread * );

#endif