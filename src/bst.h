#ifndef BST_H
#define BST_H

#include <stdlib.h>
#include <stdbool.h>
#include "deflate.h"

typedef struct _bst_t {
	byte *word;
	size_t index_s;
	size_t index_e;
	size_t word_len;
	bool is_old;
	struct _bst_t *left;
	struct _bst_t *right;
} bst_t;

bst_t *new_bst();
void delete_bst(bst_t **tree);
void insert_bst(bst_t **tree, byte *word, size_t word_len,
				size_t index_s, size_t index_e);
bst_t *search_bst(bst_t *tree, byte *word, 
				  size_t start_index, size_t end_index,
				  size_t word_size, size_t *len);
void clean_bst(bst_t **tree, size_t start_index, size_t end_index);
void delete_node_bst(bst_t **tree);
void set_bst(bst_t **tree, byte *new_word, size_t word_len,
			 size_t index_s, size_t index_e);
void print_bst(bst_t *tree);
size_t get_substr_len_bst(byte *str1, size_t str1_len, 
						  byte *str2, size_t str2_len);

#endif
