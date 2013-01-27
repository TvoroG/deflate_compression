all: deflate.o static_deflate.o cyclic_queue.o
	cc deflate.o static_deflate.o cyclic_queue.o -o deflate

deflate.o: deflate.c deflate.h
	cc -c deflate.h deflate.c

static_deflate.o: static_deflate.c static_deflate.h
	cc -c static_deflate.h static_deflate.c

cyclic_queue.o: cyclic_queue.h cyclic_queue.c
	cc -c cyclic_queue.c cyclic_queue.h

clean:
	rm *.o deflate
