#ifndef DEFLATE_H
#define DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define INPUT "input"
#define OUTPUT "output"
#define N 8

#define GetBit(reg, bit)   ((reg) & (1 << (bit)) ? 1 : 0)
#define SetBit(reg, bit)   ((reg) |= (1<<(bit)))
#define ClearBit(reg, bit) ((reg) &= (~(1<<(bit))))
#define BitIsSet(reg, bit) (((reg) & (1 << (bit))) != 0)

#define HEADER_LEN 3
#define DICT_SIZE_Q 32769 /*32768 + 1*/
#define LEN_SIZE_Q 259 /*258 + 1*/
#define DICT_MAX 32768
#define LEN_MAX 258
#define EL_SIZE 1 /* element size */

#define RANGE1_BEGINNING 0
#define RANGE1_END 143
#define RANGE1_BASE 48 /* 00110000 - 10111111 */
#define RANGE1_LEN 8
#define RANGE2_BEGINNING 144
#define RANGE2_END 255
#define RANGE2_BASE 400 /* 110010000 - 111111111 */
#define RANGE2_LEN 9
#define RANGE3_BEGINNING 256
#define RANGE3_END 279
#define RANGE3_BASE 0 /* 0000000 - 0010111 */
#define RANGE3_LEN 7
#define RANGE4_BEGINNING 280
#define RANGE4_END 287
#define RANGE4_BASE 192 /*11000000 - 11000111 */
#define RANGE4_LEN 8

#define LEN_CODE_BEGINNING 257
#define LEN_CODE_NUM 29 /* size of length_codes array */
struct {
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
	0, 258};

#define OFF_CODE_NUM 30 /* size of offset_codes array */
#define OFF_CODE_LEN 5 /* num of bits in offset code is fixed */
struct {
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
	13, 24577};

typedef unsigned char byte;
typedef unsigned short two_bytes;

void die(char *);
void print_bytes(int, size_t);
void static_deflate(FILE *, FILE *, bool);
void write_literal(FILE *, two_bytes);
void write_pointer(FILE *, size_t, size_t);
void write_static_header(FILE *, bool);
void write_huffman_code(FILE *, two_bytes, size_t);
void write_bits(FILE *, byte, size_t);
void next_bit(FILE *);
void byte_flush(FILE *);
void get_huffman_code_of_litlen(two_bytes, 
								two_bytes *, 
								size_t *);
two_bytes get_code_of_length(size_t);
void get_extra_bits_of_length(size_t, two_bytes, byte *, size_t *);
byte get_code_of_offset(size_t);
void get_extra_bits_of_offset(size_t, byte, two_bytes *, size_t *);
#endif
