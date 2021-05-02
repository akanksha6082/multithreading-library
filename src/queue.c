#include <stdio.h>
#include <stdlib.h>

#include "../include/queue.h"

int qinit(queue * q){
    
    if(q){
        q->front = q->rear = NULL;
        q->qcount = 0;
        return 0;
    }
    
    return -1;
}

int is_empty(queue * q){
    return (q->qcount == 0);
}

int return_count(queue * q){
    return q->qcount;
}

node * create_node(athread * tcb){

    node * new_node = (node*)malloc(sizeof(node));
    if(new_node == NULL)
        return NULL;
    
    new_node->tcb = tcb;
    new_node->next = NULL;

    return new_node;

}

void display(queue * q){

    node * front = q->front;
    while(front){
        printf("%5d - %d\n", front->tcb->tid,front->tcb->thread_state);
        front = front->next;
    }

    return;
}

athread * search_tcb(queue * q, athread_t thread_id){
    
    if(is_empty(q))
        return NULL;

    node * front = q->front;
    while(front){
        if(front->tcb->tid == thread_id){
            return front->tcb;
        }
        front = front->next;
    }

    return NULL;
}

void enqueue(queue * q, athread * tcb){

    node * new_node = create_node(tcb);
    if(is_empty(q)){
        q->front = q->rear = new_node;
    }
    else{

        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->qcount ++;
    return;
}

node * dequeue(queue * q){
    if(is_empty(q)){
        qinit(q);
        return NULL;
    }
    node * p = q->front->next;
    node * return_node = q->front;
    q->front = p;
    q->qcount --;
    return return_node;
    
}