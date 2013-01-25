#ifndef CYCLIC_QUEUE_H
#define CYCLIC_QUEUE_H
#include <stdbool.h>
#include "deflate.h"

typedef struct {
	byte *queue;
	size_t s;
	size_t e;
	size_t len;
} cyclic_queue;

cyclic_queue *new_cyclic_queue(size_t);
void add_cyclic_queue(cyclic_queue *, byte);
void search_cyclic_queue(cyclic_queue *, 
						 cyclic_queue *, 
						 size_t *, 
						 size_t *);
void print_cyclic_queue(cyclic_queue *);

#endif
