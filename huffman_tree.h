#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
#include "deflate.h"

typedef struct _huffman_tree {
	size_t probability;
	struct _huffman_tree *left;
	struct _huffman_tree *right;
	two_bytes code;
} huffman_tree;

int compare_huffman_tree(const void *, const void *);

#endif
