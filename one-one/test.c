#include<stdio.h>
#include "athread.h"
#include <string.h>
#include <unistd.h>

#define print(str) write(1, str, strlen(str))

void * start_routine(void * args){
    char b;
    print("Thread 1\n");
    sleep(10);
    scanf("%c", &b);
    //read(0, &b, sizeof(int));
    printf("akanksha : %c", b);
    int * a = (int*)args;
    
    return (void*)a;
}

void * start_function(void * args){
   
    print("Thread 2\n");
    sleep(5);
    printf("Im awake\n");
    return NULL;
}

int main(){

    athread_init();
    
    athread_t tid, tid2;
    athread_attr_t attr;


    athread_attr_init(&attr);
    

    int a = 10;
    printf("Enter the value of a :\n");
    scanf("%d", &a);
    void * retval;

    athread_create(&tid, &attr, start_routine, &a);
    athread_create(&tid2, &attr, start_function, &a);
    
    sleep(10);
    athread_join(tid, &retval);
    athread_join(tid2, NULL);
    printf("%d\n", *(int*)retval);

    
    return 0;
}