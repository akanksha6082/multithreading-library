#include "queue.h"

void qinit( queue * q){
    
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

int is_empty(queue * q){
    return ( q->count == 0);
}

int return_qcount(queue * q){
    return q->count;
}

node * create_node(athread * tcb){
    
    node * q = (node*)malloc(sizeof(node));
    if(q == NULL){
        return NULL;
    }

    q->next = NULL;
    q->tcb = tcb;
    return q;

}

void enqueue(queue * q, athread * tcb){
    
    node * rear = q->rear, *front = q->front;
    if(!is_empty(q)){
        rear->next = create_node(tcb);
        q->rear = rear->next;
        return;
    }
    q->rear = q->front = create_node(tcb);
    q->count ++;
    return; 

}

void dequeue(queue * q){

    node * front = q->front;

    if(is_empty(q)){
        qinit(q);
        return;
    }

    node * p = q->front->next;
    free(q->front);
    q->front = p;
    q->count --;
    return;
    
}

athread * search_tcb(queue * q, athread_t tid){
    if(is_empty(q))
        return NULL;
    node * front = q->front;
    while(front){
        if(front->tcb->tid == tid)
            return front->tcb;
        front = front->next;
    }
    return NULL;
}

void display(queue * q){
    node * front = q->front;
    while(front){
        printf("%5d", front->tcb->tid);
        front = front->next;
    }
}
