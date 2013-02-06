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
size_t generate_huffman_codes(huffman_tree *tree[],
							  size_t array_size,
							  size_t tree_size);
size_t get_max_len(size_t bl_count[], size_t size);
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
void assign_numerical_values(huffman_tree *tree[],
							 size_t array_size,
							 size_t next_code[]);
void build_tree(huffman_tree *[], size_t);
void destroy_tree(huffman_tree *[], huffman_tree *);
void sort_in_tree(huffman_tree *[], huffman_tree *, int);
void write_HLIT_HDIST(two_bytes max_litlen_code, two_bytes max_off_code);
void write_dynamic_header(bool isfinal);
size_t write_HCLEN(huffman_tree *length_tree[]);
void write_code_length_for_code_length(huffman_tree *length_tree[], 
									   size_t HCLEN);
void write_code_length_for_alphabet(huffman_tree *tree[],
									huffman_tree *length_tree[],
									size_t max_litlen_code);
void write_compressed_data(two_bytes inter_res[], 
						   size_t real_size, 
						   huffman_tree *litlen_tree[], 
						   huffman_tree *off_tree[]);
void count_probability_for_code_length(huffman_tree *litlen_tree[],
									   size_t,
									   huffman_tree *off_tree[],
									   size_t,
									   huffman_tree *length_tree[],
									   size_t *);
static size_t write_pointer(two_bytes [],
					 size_t, 
					 size_t, 
					 size_t);
#endif
