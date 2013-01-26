all: deflate.o cyclic_queue.o
	cc deflate.o cyclic_queue.o -o deflate

deflate.o: deflate.c deflate.h
	cc -c deflate.c deflate.h

cyclic_queue.o: cyclic_queue.h cyclic_queue.c
	cc -c cyclic_queue.c cyclic_queue.h
