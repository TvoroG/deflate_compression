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

void search_cyclic_queue(cyclic_queue *dict, 
						 cyclic_queue *buff, 
						 size_t *offset, 
						 size_t *length)
{
	size_t len, maxlen = 0, maxoffset = 0;
	int k, j, i = dict->e - 1;
	if (i < 0)
		i = dict->len - 1;
	while (i != dict->s) {
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
			maxoffset = i;
		}
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
	for (i = 0; i < 100; i++)
		printf("%c, ", cq->queue[i]);
	printf("\ns = %d, e = %d\n", cq->s, cq->e);
}
