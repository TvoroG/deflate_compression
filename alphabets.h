#ifndef ALPHABETS_H
#define ALPHABETS_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "deflate.h"

#define LEN_CODE_BEGINNING 257
#define LEN_CODE_NUM 29 /* size of length_codes array */
typedef struct {
	size_t extra_bits_num;
	size_t base_len;
} _length_codes_t;
extern _length_codes_t length_codes[];


#define OFF_CODE_NUM 30 /* size of offset_codes array */
#define OFF_CODE_LEN 5 /* num of bits in offset code is fixed */
typedef struct {
	size_t extra_bits_num;
	size_t base_off;
} _offset_codes_t;
extern _offset_codes_t offset_codes[];


#define HUFF_CODE_NUM 4
typedef struct {
	size_t bits_num;
	two_bytes base_lit_value;
	two_bytes base_code;
} _huffman_codes_t;
extern _huffman_codes_t huffman_codes[];


#define CODE_LENGTH_ORDER_SIZE 19
extern size_t code_length_order[];

#define REPEAT_17_BEG 3
#define REPEAT_17_END 10
#define REPEAT_18_BEG 11
#define REPEAT_18_END 138

#define END_OF_BLOCK 256

two_bytes get_code_of_length(size_t);
byte get_code_of_offset(size_t);

void get_extra_bits_of_length(size_t, 
							  two_bytes, 
							  byte *, 
							  size_t *);
void get_extra_bits_of_offset(size_t, 
							  byte, 
							  two_bytes *, 
							  size_t *);

void get_huffman_code_of_litlen(two_bytes literal, 
								two_bytes *code, 
								size_t *code_len);

#endif
