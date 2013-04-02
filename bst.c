#include "bst.h"

#include <string.h>
#include <assert.h>

static bst_t *new_node_bst(byte *word, size_t ptr);
static void inner_search_bst(bst_t *tree, byte *word, size_t word_size,
							 bst_t **res, size_t *len);
static void inner_print_bst(bst_t *tree, size_t k);
static bst_t **get_most_left_node_bst(bst_t **tree);

bst_t *new_bst()
{
	return NULL;
}

void delete_bst(bst_t **tree)
{
	if (*tree != NULL) {
		delete_bst(&(*tree)->left);
		delete_bst(&(*tree)->right);
		//free((*tree)->word);
		free(*tree);
		*tree = NULL;
	}
}

void insert_bst(bst_t **tree, byte *word, size_t ptr)
{
	if (*tree != NULL) {
		int cmp = strcmp(word, (*tree)->word);
		if (cmp > 0) {
			insert_bst(&(*tree)->right, word, ptr);
		} else if (cmp <= 0) {
			insert_bst(&(*tree)->left, word, ptr);
		}
	} else {
		*tree = new_node_bst(word, ptr);
	}
}

bst_t *search_bst(bst_t *tree, byte *word, 
				  size_t word_size, size_t *len)
{
	assert(word_size <= LEN_MAX);
	bst_t *res = NULL;
	*len = 0;
	inner_search_bst(tree, word, word_size, &res, len);
	return res;
}

static void inner_search_bst(bst_t *tree, byte *word, size_t word_size,
							 bst_t **res, size_t *len)
{
	if (tree != NULL) {
		if (*word > *tree->word) {
			inner_search_bst(tree->right, word, word_size, res, len);
		} else if (*word < *tree->word) {
			inner_search_bst(tree->left, word, word_size, res, len);
		} else {
			size_t length = get_substr_len_bst(tree->word, word);
			assert(length >= 1);
			if (length > *len) {
				*len = length;
				*res = tree;
			}
				
			if ((length < word_size - 1) && (length < LEN_MAX - 1)) {
				if (word[length + 1] > tree->word[length + 1]) {
					inner_search_bst(tree->right, word, word_size, 
									 res, len);
				} else if (word[length + 1] < tree->word[length + 1]) {
					inner_search_bst(tree->left, word, word_size,
									 res, len);
				} else {
					die("error in serch_bst\n");
				}
			}
		}
	}
}

void clean_bst(bst_t **tree, size_t start_ptr, size_t end_ptr)
{
	if (*tree != NULL) {
		clean_bst(&(*tree)->left, start_ptr, end_ptr);
		clean_bst(&(*tree)->right, start_ptr, end_ptr);
		if (start_ptr <= end_ptr) {
			if (!((*tree)->ptr >= start_ptr 
				  && (*tree)->ptr <= end_ptr)) {
				delete_node_bst(tree);
			}
		} else {
			if (!((*tree)->ptr >= start_ptr 
				  || (*tree)->ptr <= end_ptr)) {
				delete_node_bst(tree);
			}
		}
	}
}

void delete_node_bst(bst_t **tree)
{
	assert(*tree != NULL);
	bst_t *old_node;
	if ((*tree)->left == NULL && (*tree)->right == NULL) {
		old_node = *tree;
	} else if ((*tree)->left != NULL && (*tree)->right == NULL) {
		old_node = *tree;
		*tree = (*tree)->left;
	} else if ((*tree)->left == NULL && (*tree)->right != NULL) {
		old_node = *tree;
		*tree = (*tree)->right;
	} else {
		bst_t **node = get_most_left_node_bst(&(*tree)->right);
		(*node)->left = (*tree)->left;
		(*node)->right = (*tree)->right;
		old_node = *tree;
		*tree = *node;
		*node = NULL;
	}
	
	free(old_node->word);
	free(old_node);
	old_node = NULL;
}

void set_bst(bst_t *tree, byte *new_word, size_t new_ptr)
{
	assert(tree != NULL);
	free(tree->word);
	tree->word = new_word;
	tree->ptr = new_ptr;
}

void print_bst(bst_t *tree)
{
	inner_print_bst(tree, 0);
}

static void inner_print_bst(bst_t *tree, size_t k)
{
	if (tree == NULL)
		return;

	size_t i;
	for (i = 0; i < k; i++)
		printf("-");
	
	printf("%s, %d\n", tree->word, tree->ptr);
	inner_print_bst(tree->left, k + 1);
	inner_print_bst(tree->right, k + 1);
}

size_t get_substr_len_bst(byte *str1, byte *str2)
{
	size_t len = 0;
	while ((*str1 == *str2) && (*str1 != '\0') && (*str2 != '\0')) {
		str1++;
		str2++;
		len++;
	}
	return len;
}

static bst_t **get_most_left_node_bst(bst_t **tree)
{
	bst_t **bst = tree;
	while ((*bst)->left != NULL)
		bst = &(*bst)->left;
	return bst;
}

static bst_t *new_node_bst(byte *word, size_t ptr)
{
	bst_t *node = (bst_t *) malloc(sizeof(bst_t));
	node->word = word;
	node->ptr = ptr;
	node->left = NULL;
	node->right = NULL;
	return node;
}
