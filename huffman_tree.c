#include "huffman_tree.h"



int compare_huffman_tree(const void *tree1, const void *tree2)
{
	return ((huffman_tree *) tree2)->probability - 
		((huffman_tree *) tree1)->probability;
}
