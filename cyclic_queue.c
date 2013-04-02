#include <assert.h>
#include "cyclic_queue.h"

static void set_bst_cyclic_queue(cyclic_queue *cq, size_t size);
static size_t index_to_ptr(cyclic_queue *cq, size_t index);

cyclic_queue *new_cyclic_queue(size_t len, bool is_bst)
{
	cyclic_queue *cq = (cyclic_queue *) malloc(sizeof(cyclic_queue));
	cq->queue = (byte *) malloc(len);
	cq->s = 0;
	cq->e = 0;
	cq->len = len;
	cq->is_bst = is_bst;
	if (is_bst)
		cq->bst = new_bst();
	else
		cq->bst = NULL;
	return cq;
}

void delete_cyclic_queue(cyclic_queue *cq)
{
	delete_bst(&cq->bst);
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

	if (cq->is_bst)
		set_bst_cyclic_queue(cq, size);
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

/*
void search_cyclic_queue(cyclic_queue *dict, 
						 cyclic_queue *buff, 
						 size_t *offset, 
						 size_t *length)
{
	assert(dict->is_bst == true);
	size_t buff_size = size_cyclic_queue(buff);

	byte word[buff_size + 1];
	read_cyclic_queue(buff, word, 0, buff_size);
	word[buff_size] = '\0';
	bst_t *ans_bst = search_bst(dict->bst, word, buff_size, length);

	if (*length >= 3) {
		if (ans_bst->ptr < dict->e)
			*offset = dict->e - ans_bst->ptr;
		else
			*offset = dict->len - ans_bst->ptr + dict->e;
	} else {
		*offset = 0;
		*length = 0;
	}
}
*/
size_t size_cyclic_queue(cyclic_queue *cq)
{
	if (cq->s <= cq->e)
		return cq->e - cq->s;
	else
		return cq->len - cq->s + cq->e;
}

void clear_cyclic_queue(cyclic_queue *cq)
{
	cq->s = 0;
	cq->e = 0;
	if (cq->is_bst) {
		delete_bst(&cq->bst);
		cq->bst = new_bst();
	}
}

size_t read_cyclic_queue(cyclic_queue *cq, byte buff[], 
						 size_t from, size_t buff_size)
{
	assert(from < cq->len);
	size_t num = 0;
	int i;

	if (cq->s <= cq->e) {
		i = cq->s + from;
		if (i < cq->len && i < cq->e)
			for ( ; i != cq->e && num < buff_size; i++, num++)
				buff[num] = cq->queue[i];
	} else {
		i = cq->s + from;
		if (i >= cq->len)
			i = i - cq->len;

		if (i < cq->e) 
			for ( ; i != cq->e && num < buff_size; num++) {
				buff[num] = cq->queue[i];
				i++;
				if (i >= cq->len)
					i = 0;
			}
	}

	return num;
}

void get_cyclic_queue(cyclic_queue *cq, byte *buff, 
					  two_bytes length, two_bytes offset)
{
	assert(size_cyclic_queue(cq) >= length);
	assert(length <= offset);
	int i;

	if (cq->s <= cq->e) {
		i = cq->e - offset;
		assert(cq->s <= i);
	} else if (cq->s > cq->e) {
		if (offset > cq->e) {
			i = cq->len - (offset - cq->e);
			assert(cq->s <= i);
		} else {
			i = cq->e - offset;
			assert(cq->s > i);
		}
	}

	int k;
	two_bytes len;
	for (k = 0, len = length; len > 0; len--, k++) {
		buff[k] = cq->queue[i];
		i++;
		if (i >= cq->len)
			i = 0;
	}
}

void print_cyclic_queue(cyclic_queue *cq)
{
	int i;
	for (i = 0; i < cq->len; i++)
		printf("%c, ", cq->queue[i]);
	printf("\ns = %d, e = %d, val = %c\n", cq->s, cq->e, cq->queue[cq->s]);
}

static void set_bst_cyclic_queue(cyclic_queue *cq, size_t count)
{
	size_t cq_size = size_cyclic_queue(cq);
	size_t k;

	if (cq_size > LEN_MAX) {
		if (cq_size - LEN_MAX >= count)
			k = count;
		else
			k = cq_size - LEN_MAX;
	} else {
		k = 0;
	}

	if (cq_size >= cq->len - 1) {
		int end_ptr = cq->e - 1;
		if (end_ptr < 0)
			end_ptr = cq->len - 1;
		clean_bst(&cq->bst, cq->s, end_ptr);
	}


	if (k != 0) {
		size_t i;
		for (i = 0; i < k; i++) {
			size_t start = cq_size - (LEN_MAX + i);
			byte *word = (byte *) malloc(LEN_MAX + 1);
			size_t num_read = read_cyclic_queue(cq, word, 
												start, LEN_MAX);
			word[LEN_MAX] = '\0';
			insert_bst(&cq->bst, word, index_to_ptr(cq, start));
			assert(num_read == LEN_MAX);
		}
	} else {
		byte *word = malloc(cq_size + 1);
		size_t num_read = read_cyclic_queue(cq, word, 0, cq_size);
		word[cq_size] = '\0';
		set_bst(cq->bst, word, index_to_ptr(cq, 0));
	}
}


static size_t index_to_ptr(cyclic_queue *cq, size_t index)
{
	size_t offset;
	if (cq->s <= cq->e) {
		offset = cq->s + index;
		assert(offset < cq->e);
	} else {
		offset = cq->s + index;
		if (offset >= cq->len)
			offset = offset - cq->len;
		assert(offset < cq->e);
	}
	return offset;
}
