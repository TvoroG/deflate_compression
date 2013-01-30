#ifndef ALPHABETS_H
#define ALPHABETS_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "deflate.h"

#define LEN_CODE_BEGINNING 257
#define LEN_CODE_NUM 29 /* size of length_codes array */
static struct {
	size_t extra_bits_num;
	size_t base_len;
} length_codes[] = {
	0, 3,
	0, 4,
	0, 5,
	0, 6,
	0, 7,
	0, 8,
	0, 9,
	0, 10,
	1, 11,
	1, 13,
	1, 15,
	1, 17,
	2, 19,
	2, 23,
	2, 27,
	2, 31,
	3, 35,
	3, 43,
	3, 51,
	3, 59,
	4, 67,
	4, 83,
	4, 99,
	4, 115,
	5, 131,
	5, 163,
	5, 195,
	5, 227,
	0, 258
};

#define OFF_CODE_NUM 30 /* size of offset_codes array */
#define OFF_CODE_LEN 5 /* num of bits in offset code is fixed */
static struct {
	size_t extra_bits_num;
	size_t base_off;
} offset_codes[] = {
	0, 1,
	0, 2,
	0, 3,
	0, 4,
	1, 5,
	1, 7,
	2, 9,
	2, 13,
	3, 17,
	3, 25,
	4, 33,
	4, 49,
	5, 65,
	5, 97,
	6, 129,
	6, 193,
	7, 257,
	7, 385,
	8, 513,
	8, 769,
	9, 1025,
	9, 1537,
	10, 2049,
	10, 3073,
	11, 4097,
	11, 6145,
	12, 8193,
	12, 12289,
	13, 16385,
	13, 24577
};

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


#endif
