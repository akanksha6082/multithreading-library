#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "athread.h"

long int c=0 , c1=0, c2=0, c3=0 , run =1;

athread_spinlock_t spinlock;

void * f1(void * args){
    
    printf("f1\n");
    while(run == 1){

        c1 ++;
        athread_spin_lock(&spinlock);
        c++;
        athread_spin_unlock(&spinlock);
      
    }
  
    athread_exit(NULL);

}

void * f2(void * args){
    
    printf("f2\n");
    while(run == 1){
        
        c2 ++;
        athread_spin_lock(&spinlock);
        c++;
        athread_spin_unlock(&spinlock);
    }

    athread_exit(NULL);
}

void * f3(void * args){
    
    printf("f3\n");
    while(run == 1){ 
        c3 ++;
        athread_spin_lock(&spinlock);
        c++;
        athread_spin_unlock(&spinlock);
    }

    athread_exit(NULL);
}
void signal_handler(int signum){
    printf("f4 recieved sigint signal");
}

void * f4(void * args){
    
    printf("f4\n");
    signal(SIGSEGV, signal_handler);
    while(1);
}

int main(int argc, char * argv[]){
    
    athread_init();
    athread_spin_init(&spinlock);

    athread_t t1, t2, t3, t4;
    
    athread_create(&t3, f3, NULL);
    athread_create(&t2, f2, NULL);
    athread_create(&t1, f1, NULL);
    athread_create(&t4, f4, NULL);

    for(int i=0; i<100000000; i++){
        if(i == 100000){
            athread_kill(t4, SIGSEGV);
        }
    }

    run = 0;

    athread_join(t1, NULL);
    athread_join(t2, NULL);
    athread_join(t3, NULL);

    printf("\nc = %ld c1 = %ld c2 = %ld  c3 = %ld\n",c, c1, c2, c3);

     for(int i=0; i<100000000; i++){
        if(i == 100000){
            printf("raised signal for f4\n");
            athread_kill(t4, SIGSEGV);
        }
    }

    printf("\nexiting from main thread\n");

    athread_spin_destroy(&spinlock);
    return 0;

}
