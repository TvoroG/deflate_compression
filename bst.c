#include "bst.h"

#include <string.h>
#include <assert.h>

#include "deflate.h"

static bst_t *new_node_bst(char *word, size_t offset);
static void inner_search_bst(bst_t *tree, char *word, size_t word_size,
							 bst_t **res, size_t *len);
static void inner_print_bst(bst_t *tree, size_t k);

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

void insert_bst(bst_t **tree, char *word, size_t offset)
{
	if (*tree != NULL) {
		int cmp = strcmp(word, (*tree)->word);
		if (cmp > 0) {
			insert_bst(&(*tree)->right, word, offset);
		} else if (cmp <= 0) {
			insert_bst(&(*tree)->left, word, offset);
		}
	} else {
		*tree = new_node_bst(word, offset);
	}
}

bst_t *search_bst(bst_t *tree, char *word, 
				  size_t word_size, size_t *len)
{
	assert(word_size <= LEN_MAX);
	bst_t *res = NULL;
	*len = 0;
	inner_search_bst(tree, word, word_size, &res, len);
	return res;
}

static void inner_search_bst(bst_t *tree, char *word, size_t word_size,
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
	
	printf("%s, %d\n", tree->word, tree->offset);
	inner_print_bst(tree->left, k + 1);
	inner_print_bst(tree->right, k + 1);
}

size_t get_substr_len_bst(char *str1, char *str2)
{
	size_t len = 0;
	while ((*str1 == *str2) && (*str1 != '\0') && (*str2 != '\0')) {
		str1++;
		str2++;
		len++;
	}
	return len;
}

static bst_t *new_node_bst(char *word, size_t offset)
{
	bst_t *node = (bst_t *) malloc(sizeof(bst_t));
	node->word = word;
	node->offset = offset;
	node->left = NULL;
	node->right = NULL;
	return node;
}
