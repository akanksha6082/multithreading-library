#include<stdio.h>
#include "athread.h"
#include <string.h>
#include <unistd.h>

#define print(str) write(1, str, strlen(str))

void * start_routine(void * args){
    print("Thread 1\n");
    int * a = (int*)args;
    
    return (void*)a;
}

void * start_function(void * args){
    print("Thread 2\n");
    athread_t tid = athread_self();
    athread_detach(tid);
    print("Thread 2 - exiting\n");
    return NULL;    
}

int main(){

    athread_init();
    
    athread_t tid, tid2;
    athread_attr_t attr;


    athread_attr_init(&attr);
    

    int a = 10;
    void * retval;

    athread_create(&tid, &attr, start_routine, &a);
    athread_create(&tid2, &attr, start_function, NULL);

    sleep(10);
    athread_join(tid, &retval);
    printf("%d\n", *(int*)retval);
    athread_join(tid2, &retval);
    
    return 0;
}