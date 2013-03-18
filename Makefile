CFLAGS=-g -Wall -Isrc -Wall -Wextra

all: deflate.o static_deflate.o dynamic_deflate.o nocompress_deflate.o writer.o alphabets.o cyclic_queue.o huffman_tree.o inflate.o reader.o tests.o static_inflate.o dynamic_inflate.o nocompress_inflate.o
	cc deflate.o static_deflate.o dynamic_deflate.o nocompress_deflate.o writer.o alphabets.o cyclic_queue.o huffman_tree.o inflate.o reader.o tests.o static_inflate.o dynamic_inflate.o nocompress_inflate.o -pthread -o deflate

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
	rm -f *.o
	rm -f *~
	rm -f *.h.gch
	rm -f deflate


inflate.o: inflate.h inflate.c
	cc -c inflate.h inflate.c

reader.o: reader.c reader.h
	cc -c reader.c reader.h

static_inflate.o: static_inflate.c static_inflate.h
	cc -c static_inflate.c static_inflate.h

dynamic_inflate.o: dynamic_inflate.c dynamic_inflate.h
	cc -c dynamic_inflate.c dynamic_inflate.h

nocompress_inflate.o: nocompress_inflate.c nocompress_inflate.h
	cc -c nocompress_inflate.c nocompress_inflate.h

tests.o: tests.h tests.c
	cc -c tests.c tests.h
