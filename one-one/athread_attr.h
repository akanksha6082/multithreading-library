#ifndef __ATHREAD_ATTR_H
#define __ATHREAD_ATTR_H

#include "athread_types.h"
#include "utils.h"

int athread_attr_init(athread_attr_t * attr);
int athread_attr_destroy(athread_attr_t *attr);

/*getter and setters*/

int athread_attr_setdetachstate(athread_attr_t *attr, int detachstate);
int athread_attr_getdetachstate(athread_attr_t *attr, int * detachstate);

int athread_attr_setstacksize(athread_attr_t *attr, size_t stacksize);
int athread_attr_getstacksize(athread_attr_t *attr, size_t *stacksize);


int athread_attr_setstack(athread_attr_t *attr, void * stackaddr, size_t stacksize);
int athread_attr_getstack(athread_attr_t *attr, void ** stackaddr, size_t *stacksize);


#endif