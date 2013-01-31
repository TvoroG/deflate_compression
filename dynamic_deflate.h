#ifndef DYNAMIC_DEFLATE_H
#define DYNAMIC_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include "deflate.h"
#include "cyclic_queue.h"
#include "huffman_tree.h"

extern FILE *input;
extern FILE *output;
extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void dynamic_deflate(off_t, bool);
size_t LZ77(two_bytes [], size_t);
void generate_huffman_codes(huffman_tree *tree[],
							size_t array_size,
							size_t tree_size);
void init_tree(huffman_tree *[], size_t);
void count_probability_for_litlen(huffman_tree *tree[], 
								  two_bytes inter_res[], 
								  size_t real_size, 
								  size_t *tree_size,
								  two_bytes *max_code);
void count_probability_for_offset(huffman_tree *tree[], 
								  two_bytes inter_res[], 
								  size_t real_size, 
								  size_t *tree_size, 
								  two_bytes *max_code);


void find_numerical_values(size_t bl_count[], 
						   size_t size, 
						   size_t next_code[]);
void assign_numerical_values(huffman_tree *tree, 
							 size_t next_code[]);
void build_tree(huffman_tree *[], size_t);
void destroy_tree(huffman_tree *[], huffman_tree *);
void sort_in_tree(huffman_tree *[], huffman_tree *, int);
size_t write_pointer(two_bytes [],
					 size_t, 
					 size_t, 
					 size_t);
#endif
