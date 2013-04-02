#ifndef BST_H
#define BST_H

#include <stdlib.h>
#include "deflate.h"

typedef struct _bst_t {
	byte *word;
	size_t ptr;
	struct _bst_t *left;
	struct _bst_t *right;
} bst_t;

bst_t *new_bst();
void delete_bst(bst_t **tree);
void insert_bst(bst_t **tree, byte *word, size_t ptr);
bst_t *search_bst(bst_t *tree, byte *word, 
				  size_t word_size, size_t *len);
void clean_bst(bst_t **tree, size_t start_ptr, size_t end_ptr);
void delete_node_bst(bst_t **tree);
void set_bst(bst_t *tree, byte *new_word, size_t new_offset);
void print_bst(bst_t *tree);
size_t get_substr_len_bst(byte *str1, byte *str2);

#endif
