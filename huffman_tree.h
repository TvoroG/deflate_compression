#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
#include "deflate.h"

typedef size_t prob_t;
typedef struct _huffman_tree {
	prob_t probability;
	two_bytes code;
	size_t len;
	two_bytes huff_code;
	struct _huffman_tree *left;
	struct _huffman_tree *right;
} huffman_tree;

huffman_tree *new_huffman_tree(prob_t, 
							   two_bytes, 
							   huffman_tree *, 
							   huffman_tree *);

void count_len_huffman_tree(huffman_tree *, size_t [], size_t);
int compare_huffman_tree(const void *, const void *);
void print_huffman_tree(huffman_tree *);

#endif
