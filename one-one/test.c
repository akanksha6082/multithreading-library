#include<stdio.h>
#include "athread.h"
#include <string.h>
#include <unistd.h>

#define print(str) write(1, str, strlen(str))


// void * start_routine(void * args){
//     print("Thread 1\n");
//     int * a = (int*)args;
    
//     return (void*)a;
// }

// void * start_function(void * args){
//     print("Thread 2\n");
//     print("Thread 2 - exiting\n");

//     athread_exit((void*)1);
// }

// int main(){

//     athread_init();
    
//     athread_t tid, tid2;
//     athread_attr_t attr;


//     athread_attr_init(&attr);
    

//     int a = 10;
//     void * retval;

//     athread_create(&tid, &attr, start_routine, &a);
//     athread_create(&tid2, &attr, start_function, NULL);

//     sleep(10);
//     athread_join(tid, &retval);
//     printf("%d\n", *(int*)retval);
//     athread_join(tid2, &retval);
    
//     return 0;
// }

int c = 0;
void * f1(void *);
void * f2(void *);

void * f1(void * args){
    print("f1\n");
    return NULL;
}

void * f2(void * args){
    print("f2\n");
    return NULL;
}
int main(){
    athread_init();

    athread_t t1, t2;

    athread_create(&t1, NULL, f1, NULL);
    athread_create(&t2, NULL, f2, NULL);

    athread_join(t1, NULL);
    athread_join(t2, NULL);

}