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

typedef unsigned char byte;
typedef unsigned short two_bytes;

void die(char *);
void print_bytes(int, size_t);
void static_deflate(FILE *, FILE *, bool);
void write_literal(FILE *, byte);
void write_pointer(FILE *, size_t, size_t);
void write_static_header(FILE *, bool);
void next_bit(FILE *);
void byte_flush(FILE *);
void get_huffman_code_for_litlen_alphabet(two_bytes, 
										  two_bytes *, 
										  size_t *);

#endif
