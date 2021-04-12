target: test.o athread.o queue.o stackmem.o timer.o utils.o athread_locks.o
		cc test.o athread.o queue.o stackmem.o timer.o utils.o athread_locks.o -o thread
test.o: test.c athread.h
		cc -c test.c
athread.o : athread.c athread.h athread_types.h utils.h queue.h stackmem.h timer.h mangle.h
		cc -c athread.c
queue.o : queue.c queue.h athread_types.h
		cc -c queue.c
stackmem.o : stackmem.c stackmem.h utils.h athread_types.h
		cc -c stackmem.c
athread_locks.o: athread.h athread_types.h
		cc -c athread_locks.c
utils.o : utils.c utils.h athread_types.h
		cc -c utils.c
timer.o : timer.c timer.h
		cc -c timer.c

clean:
		rm -f *.o thread