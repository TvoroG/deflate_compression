#ifndef BST_H
#define BST_H

#include <stdlib.h>

typedef struct _bst_t {
	char *word;
	size_t offset;
	struct _bst_t *left;
	struct _bst_t *right;
} bst_t;

bst_t *new_bst();
void delete_bst(bst_t **tree);
void insert_bst(bst_t **tree, char *word, size_t offset);
bst_t *search_bst(bst_t *tree, char *word, 
				  size_t word_size, size_t *len);
void print_bst(bst_t *tree);
size_t get_substr_len_bst(char *str1, char *str2);

#endif
