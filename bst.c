#include "bst.h"

#include <string.h>
#include <assert.h>

static bst_t *new_node_bst(byte *word, size_t index_s, size_t index_e);
static void inner_search_bst(bst_t *tree, byte *word, size_t word_size,
							 size_t start_index, size_t end_index,
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

void insert_bst(bst_t **tree, byte *word, size_t index_s, size_t index_e)
{
	if (*tree != NULL) {
		int cmp = strcmp(word, (*tree)->word);
		if (cmp > 0) {
			insert_bst(&(*tree)->right, word, index_s, index_e);
		} else if (cmp <= 0) {
			insert_bst(&(*tree)->left, word, index_s, index_e);
		}
	} else {
		*tree = new_node_bst(word, index_s, index_e);
	}
}

bst_t *search_bst(bst_t *tree, byte *word, 
				  size_t start_index, size_t end_index,
				  size_t word_size, size_t *len)
{
	assert(word_size <= LEN_MAX);
	bst_t *res = NULL;
	*len = 0;
	inner_search_bst(tree, word, word_size, 
					 start_index, end_index, &res, len);
	return res;
}

static void inner_search_bst(bst_t *tree, byte *word, size_t word_size,
							 size_t start_index, size_t end_index,
							 bst_t **res, size_t *len)
{
	if (tree != NULL) {
		if (tree->index_s <= end_index && tree->index_e >= end_index 
			&& !tree->is_old) {
			tree->is_old = true;
		}

		if (*word > *tree->word) {
			inner_search_bst(tree->right, word, word_size, 
							 start_index, end_index,
							 res, len);
		} else if (*word < *tree->word) {
			inner_search_bst(tree->left, word, word_size, 
							 start_index, end_index,
							 res, len);
		} else if (!tree->is_old) {
			size_t length = get_substr_len_bst(tree->word, word);
			assert(length >= 1);
			if (length > *len) {
				*len = length;
				*res = tree;
			}
				
			if ((length < word_size) && (length < LEN_MAX)) {
				if (word[length] > tree->word[length]) {
					inner_search_bst(tree->right, word, word_size, 
									 start_index, end_index,
									 res, len);
				} else if (word[length] < tree->word[length]) {
					inner_search_bst(tree->left, word, word_size,
									 start_index, end_index,
									 res, len);
				} else {
					die("error in search_bst\n");
				}
			}
		} else {
			inner_search_bst(tree->left, word, word_size, 
							 start_index, end_index,
							 res, len);
			inner_search_bst(tree->right, word, word_size, 
							 start_index, end_index,
							 res, len);
		}
	}
}

void clean_bst(bst_t **tree, size_t start_index, size_t end_index)
{
	if (*tree != NULL) {
		clean_bst(&(*tree)->left, start_index, end_index);
		clean_bst(&(*tree)->right, start_index, end_index);
		if (start_index <= end_index) {
			if (!((*tree)->index_s >= start_index 
				  && (*tree)->index_s <= end_index)) {
				delete_node_bst(tree);
			}
		} else {
			if (!((*tree)->index_s >= start_index 
				  || (*tree)->index_s <= end_index)) {
				delete_node_bst(tree);
			}
		}
	}
}

void delete_node_bst(bst_t **tree)
{
	assert(*tree != NULL);
	if ((*tree)->left == NULL && (*tree)->right == NULL) {
//		free((*tree)->word);
		free(*tree);
		*tree = NULL;
	} else if ((*tree)->left != NULL && (*tree)->right == NULL) {
		bst_t *old_node = *tree;
		*tree = (*tree)->left;
//		free(old_node->word);
		free(old_node);
	} else if ((*tree)->left == NULL && (*tree)->right != NULL) {
		bst_t *old_node = *tree;
		*tree = (*tree)->right;
//		free(old_node->word);
		free(old_node);
	} else {
		bst_t **node = get_most_left_node_bst(&(*tree)->right);
		(*node)->left = (*tree)->left;
		if (*node != (*tree)->right) {
			(*node)->right = (*tree)->right;
		}
		bst_t *old_node = *tree;
		*tree = *node;
		*node = NULL;
//		free(old_node->word);
		free(old_node);
	}
}

void set_bst(bst_t **tree, byte *new_word, 
			 size_t index_s, size_t index_e)
{
	if (*tree == NULL) {
		*tree = new_node_bst(new_word, index_s, index_e);
	} else {
		free((*tree)->word);
		(*tree)->word = new_word;
		(*tree)->index_s = index_s;
		(*tree)->index_e = index_e;
	}
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
	
	printf("%s, %d\n", tree->word, tree->index_s);
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

static bst_t *new_node_bst(byte *word, size_t index_s, size_t index_e)
{
	bst_t *node = (bst_t *) malloc(sizeof(bst_t));
	node->word = word;
	node->index_s = index_s;
	node->index_e = index_e;
	node->is_old = false;
	node->left = NULL;
	node->right = NULL;
	return node;
}
