CFLAGS=-g -Wall -Isrc -Wall -Wextra

all: deflate.o static_deflate.o dynamic_deflate.o nocompress_deflate.o writer.o alphabets.o cyclic_queue.o huffman_tree.o inflate.o reader.o tests.o
	cc deflate.o static_deflate.o dynamic_deflate.o nocompress_deflate.o writer.o alphabets.o cyclic_queue.o huffman_tree.o inflate.o reader.o tests.o -pthread -o deflate

deflate.o: deflate.c deflate.h
	cc -c deflate.h deflate.c

static_deflate.o: static_deflate.c static_deflate.h
	cc -c static_deflate.h static_deflate.c

dynamin_deflate.o: dynamic_deflate.c dynamic_deflate.h
	cc -c dynamic_deflate.h dynamic_deflate.c

nocompress_deflate.o: nocompress_deflate.c nocompress_deflate.h
	cc -c nocompress_deflate.c nocompress_deflate.h

alphabets.o: alphabets.c alphabets.h
	cc -c alphabets.c alphabets.h

cyclic_queue.o: cyclic_queue.h cyclic_queue.c
	cc -c cyclic_queue.c cyclic_queue.h

huffman_tree.o: huffman_tree.c huffman_tree.h
	cc -c huffman_tree.c huffman_tree.h

writer.o: writer.c writer.h
	cc -c writer.c writer.h

clean:
	rm *.o deflate

inflate: inflate.o reader.o

inflate.o: inflate.h inflate.c
	cc -c inflate.h inflate.c

reader.o: reader.c reader.h
	cc -c reader.c reader.h

tests.o: tests.h tests.c
	cc -c tests.c tests.h
