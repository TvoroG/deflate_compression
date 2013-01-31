#include "huffman_tree.h"


huffman_tree *new_huffman_tree(prob_t prob, 
							   two_bytes code, 
							   huffman_tree *left, 
							   huffman_tree *right)
{
	huffman_tree *tree = (huffman_tree *) malloc(sizeof(huffman_tree));
	tree->probability = prob;
	tree->code = code;
	tree->len = 0;
	tree->huff_code = 0;
	tree->left = left;
	tree->right = right;
	return tree;
}

void count_len_huffman_tree(huffman_tree *tree, 
							size_t bl_count[], 
							size_t len)
{
	if (tree->left == NULL && tree->right == NULL) {
		tree->len = len;
		bl_count[len]++;
	} else {
		len++;
		count_len_huffman_tree(tree->left, bl_count, len);
		count_len_huffman_tree(tree->right, bl_count, len);
	}
}


int compare_huffman_tree(const void *tree1, const void *tree2)
{
	return (*((huffman_tree **) tree2))->probability - 
		(*((huffman_tree **) tree1))->probability;
}

void print_huffman_tree(huffman_tree *tree)
{
	if (tree->left == NULL) {
		printf("len = %d, code = ", tree->len);
		print_bytes(tree->huff_code, sizeof(two_bytes));
		printf(";");
	} else {
		print_huffman_tree(tree->left);
		printf("code = %d, len = %d; ", tree->code, tree->len);
		print_huffman_tree(tree->right);
	}
}
