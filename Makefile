all: deflate.o cyclic_queue.o
	gcc deflate.o cyclic_queue.o -o deflate

deflate.o: deflate.c deflate.h cyclic_queue.h
	gcc -c deflate.c deflate.h cyclic_queue.h

cyclic_queue.o: cyclic_queue.h cyclic_queue.c deflate.h
	gcc -c cyclic_queue.c cyclic_queue.h
