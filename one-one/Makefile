target : test.o athread.o utils.o athread_attr.o stackmem.o queue.o athread_locks.o
	cc test.o athread.o utils.o athread_attr.o stackmem.o queue.o athread_locks.o -o thread

test.o : test.c athread.h
	cc -c test.c
athread.o : athread.c athread.h athread_attr.h utils.h stackmem.h queue.h athread_types.h
	cc -c athread.c
athread_attr.o : athread_attr.c athread_attr.h utils.h athread_types.h
	cc -c athread_attr.c
utils.o : utils.c utils.h athread_types.h
	cc -c utils.c
stackmem.o : stackmem.c stackmem.h athread_types.h utils.h
	cc -c stackmem.c
queue.o : queue.h queue.c athread_types.h
	cc -c queue.c
athread_locks.o : athread_locks.c athread.h athread_attr.h utils.h stackmem.h queue.h athread_types.h
	cc -c athread_locks.c
clean:
	rm -f *.o thread