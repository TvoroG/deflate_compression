all: deflate.o static_deflate.o dynamic_deflate.o alphabets.o cyclic_queue.o
	cc deflate.o static_deflate.o dynamic_deflate.o alphabets.o cyclic_queue.o -o deflate

deflate.o: deflate.c deflate.h
	cc -c deflate.h deflate.c

static_deflate.o: static_deflate.c static_deflate.h
	cc -c static_deflate.h static_deflate.c

dynamin_deflate.o: dynamic_deflate.c dynamic_deflate.h
	cc -c dynamic_deflate.h dynamic_deflate.c

alphabets.o: alphabets.c alphabets.h
	cc -c alphabets.c alphabets.h

cyclic_queue.o: cyclic_queue.h cyclic_queue.c
	cc -c cyclic_queue.c cyclic_queue.h

clean:
	rm *.o deflate
