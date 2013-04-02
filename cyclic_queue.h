#ifndef CYCLIC_QUEUE_H
#define CYCLIC_QUEUE_H
#include <stdbool.h>
#include "deflate.h"
#include "bst.h"

typedef struct {
	byte *queue;
	size_t s;
	size_t e;
	size_t len;
	bool is_bst;
	bst_t *bst;
} cyclic_queue;

cyclic_queue *new_cyclic_queue(size_t, bool);
void delete_cyclic_queue(cyclic_queue *);
void add_cyclic_queue(cyclic_queue *, byte);
void push_back_cyclic_queue(cyclic_queue *, byte *, size_t);
byte front_cyclic_queue(cyclic_queue *);
bool isempty_cyclic_queue(cyclic_queue *);
byte pop_front_cyclic_queue(cyclic_queue *);
void move_front_cyclic_queue(cyclic_queue *, cyclic_queue *, size_t);
void search_cyclic_queue(cyclic_queue *, 
						 cyclic_queue *, 
						 size_t *, 
						 size_t *);
size_t size_cyclic_queue(cyclic_queue *);
void clear_cyclic_queue(cyclic_queue *);
size_t read_cyclic_queue(cyclic_queue *cq, byte buff[],
						 size_t from, size_t buff_size);
void get_cyclic_queue(cyclic_queue *cq, byte *buff, 
					  two_bytes length, two_bytes offset);
void print_cyclic_queue(cyclic_queue *);

#endif
