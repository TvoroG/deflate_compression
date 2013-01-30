#ifndef STATIC_DEFLATE_H
#define STATIC_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "deflate.h"
#include "cyclic_queue.h"

#define HUFF_CODE_NUM 4
static struct {
	size_t bits_num;
	two_bytes base_lit_value;
	two_bytes base_code;	
} huffman_codes[] = {
	8, 0, 48,
	9, 144, 400,
	7, 256, 0,
	8, 280, 192
};

extern FILE *input;
extern FILE *output;
extern byte write_b;
extern size_t write_i;
extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void static_deflate(bool);

static void write_literal(two_bytes);
static void write_pointer(size_t, size_t);
static void write_static_header(bool);
static void write_huffman_code(two_bytes, size_t);
static void write_bits(two_bytes, size_t);
static void next_bit();
static void byte_flush();

static void get_huffman_code_of_litlen(two_bytes, 
								two_bytes *, 
								size_t *);

#endif
