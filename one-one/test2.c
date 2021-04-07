#include "athread.h"

void * f1(void * args){
    printf("thread with function f1()\n");
    int i=1000;
    while(i>=0){
        i--;
    }
    return NULL;
}

void * f2(void * args){

    printf("thread with function f2() - %d\n", athread_self());
  
    printf("finished f2() \n");

    athread_exit(NULL);
}

int main(int argc, char ** argv){
    athread_t tid, pid;


    athread_create(&tid, NULL, f1, NULL);
    athread_create(&pid, NULL, f2, NULL);
    athread_detach(pid);
    athread_join(tid, NULL);
    printf("main function is done !!\n");

    return 0;
    
}