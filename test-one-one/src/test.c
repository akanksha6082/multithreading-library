#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "athread.h"

void * f2(void * args){
    int i = 5;
    int a = 300;
    void * p = &a;
    while(i){
        printf("In f2()\n");
        i--;
    }

    athread_exit(p);

}

void * f1(void * args){
    athread_t tid;
    athread_create(&tid, NULL, f2, NULL);

    int a = 200;
    void * p = &a;
    void * ret;
    int i =5;
    while(i){
        printf("In f1()\n");
        i--;
    }

    athread_join(tid, &ret);
    printf("return value from f2 = %d\n", *(int*)ret);

    athread_exit(p);
}

int main(int argc, char ** argv){
    athread_init();

    athread_t tid;
    void * ret;
    athread_create(&tid, NULL, f1, NULL);

    int i =5;
    while(i){
        printf("In main\n");
        i--;
    }

    athread_join(tid, &ret);
    printf("return value from f1 = %d\n", *(int*)ret);

    return 0;
}