#ifndef READER_H
#define READER_H
#include <stdio.h>
#include <stdbool.h>
#include "deflate.h"

typedef enum {NO_C, STATIC_C, DYNAMIC_C, ERROR_C} compress_t;

typedef struct _reader_t {
	char *input_name;
	FILE *input;
	FILE *output;
	byte read_b;
	byte read_i;
	bool isfinal;
	compress_t compress_type;
} reader_t;

typedef struct {
	size_t code_len;
	size_t code;
} huffman_code;

void init_reader(reader_t **reader);
void delete_reader(reader_t **reader);
void read_block_header(reader_t *reader);
two_bytes decode_next_litlen(reader_t *reader);
two_bytes decode_length(reader_t *reader, two_bytes lencode);
two_bytes decode_distance(reader_t *reader, two_bytes offset_code);
byte read_HLIT(reader_t *reader);
byte read_HDIST(reader_t *reader);
byte read_HCLEN(reader_t *reader);
two_bytes read_LEN_and_NLEN(reader_t *reader);
void read_code_length_for_code_length(reader_t *reader, 
									  huffman_code cl_for_cl[],
									  size_t size);
void read_code_length_for_litlen(reader_t *reader, 
								 huffman_code alphabet_codes[], 
								 size_t alphabet_size, 
								 huffman_code cl_for_cl[], 
								 size_t hclen);
size_t read_next_huffman_code(reader_t *reader, 
							  huffman_code cl_for_cl[], 
							  size_t cl_len);
void read_next_bit(reader_t *reader);
void read_next_byte(reader_t *reader);
size_t read_bits(reader_t *reader, size_t num);
void ignore_byte(reader_t *reader);
bool is_in_huffman_code(two_bytes code, int index);

#endif
