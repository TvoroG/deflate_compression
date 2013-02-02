#include <assert.h>
#include "cyclic_queue.h"

cyclic_queue *new_cyclic_queue(size_t len)
{
	cyclic_queue *cq = (cyclic_queue *) malloc(sizeof(cyclic_queue));
	cq->queue = (byte *) malloc(len);
	cq->s = 0;
	cq->e = 0;
	cq->len = len;
	return cq;
}

void delete_cyclic_queue(cyclic_queue *cq)
{
	free(cq->queue);
	free(cq);
}

void add_cyclic_queue(cyclic_queue *cq, byte b)
{
	cq->queue[cq->e] = b;
	if (cq->e + 1 >= cq->len) {
		cq->e = 0;
		if (cq->s == cq->e)
			cq->s++;		
	} else if (cq->e + 1 == cq->s) {
		cq->e++;
		cq->s++;
		if (cq->s >= cq->len) {
			cq->s = 0;
		}
	} else {
		cq->e++;
	}
}

void push_back_cyclic_queue(cyclic_queue *cq, byte *bs, size_t size)
{
	int i;
	for (i = 0; i < size; i++) {
		add_cyclic_queue(cq, bs[i]);
	}
}

byte front_cyclic_queue(cyclic_queue *cq)
{
	assert(cq->s != cq->e);
	return cq->queue[cq->s];
}

bool isempty_cyclic_queue(cyclic_queue *cq)
{
	return cq->s == cq->e ? true : false;
}

byte pop_front_cyclic_queue(cyclic_queue *cq)
{
	assert(cq->s != cq->e);

	byte res = cq->queue[cq->s];
	cq->s++;
	if (cq->s >= cq->len)
		cq->s = 0;
	return res;
}

void move_front_cyclic_queue(cyclic_queue *from, 
							 cyclic_queue *to, 
							 size_t len)
{
	byte res;
	int num;
	for (num = 0; num < len && from->s != from->e; num++) {
		res = pop_front_cyclic_queue(from);
		add_cyclic_queue(to, res);
	}
}

void search_cyclic_queue(cyclic_queue *dict, 
						 cyclic_queue *buff, 
						 size_t *offset, 
						 size_t *length)
{
	size_t len, off = 1, maxlen = 0, maxoffset = 0;
	int k, j, i = dict->e - 1, end_i = dict->s - 1;
	if (i < 0)
		i = dict->len - 1;
	if (end_i < 0)
		end_i = dict->len - 1;

	while (i != end_i) {
		len = 0;
		for (k = i, j = buff->s; k != dict->e && j != buff->e; ) {
			if (dict->queue[k] != buff->queue[j])
				break;
			else
				len++;
			k++;
			if (k >= dict->len) k = 0;
			j++;
			if (j >= buff->len) j = 0;
		}
		if (len >= 3 && len >= maxlen) {
			maxlen = len;
			maxoffset = off;
		}
		off++;
		i--;
		if (i < 0)
			i = dict->len - 1;
	}
	*offset = maxoffset;
	*length = maxlen;
}

void print_cyclic_queue(cyclic_queue *cq)
{
	int i;
	for (i = 0; i < cq->len; i++)
		printf("%c, ", cq->queue[i]);
	printf("\ns = %d, e = %d, val = %c\n", cq->s, cq->e, cq->queue[cq->s]);
}
