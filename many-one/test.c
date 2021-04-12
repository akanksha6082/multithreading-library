#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "athread.h"

#define print(str) (write(1, str, strlen(str)))

long int c=0 , c1=0, c2=0, c3=0 , run =1;

athread_spinlock_t spinlock;

void * f1(void * args){
    
    while(run == 1){

        c1 ++;
        athread_spin_lock(&spinlock);
        c++;
        athread_spin_unlock(&spinlock);
      
    }
  
    athread_exit(NULL);

}

void * f2(void * args){
    
    while(run == 1){
        
        c2 ++;
        athread_spin_lock(&spinlock);
        c++;
        athread_spin_unlock(&spinlock);
    }

    athread_exit(NULL);
}

void * f3(void * args){
    
    while(run == 1){ 
        c3 ++;
        athread_spin_lock(&spinlock);
        c++;
        athread_spin_unlock(&spinlock);
    }

    athread_exit(NULL);
}

int main(int argc, char * argv[]){
    
    athread_init();
    athread_spin_init(&spinlock);

    athread_t t1, t2, t3;
    
    athread_create(&t3, f3, NULL);
    athread_create(&t2, f2, NULL);
    athread_create(&t1, f1, NULL);

    for(int i=0; i<100000000; i++);

    run = 0;

    athread_join(t1, NULL);
    athread_join(t2, NULL);
    athread_join(t3, NULL);

    printf("c = %ld c1 = %ld c2 = %ld  c3 = %ld\n",c, c1, c2, c3);

    athread_spin_destroy(&spinlock);
    return 0;

}
