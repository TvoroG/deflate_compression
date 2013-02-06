#ifndef DEFLATE_H
#define DEFLATE_H
#include <stdio.h>
#include <stdlib.h>

#define GetBit(reg, bit)   ((reg) & (1 << (bit)) ? 1 : 0)
#define SetBit(reg, bit)   ((reg) |= (1<<(bit)))
#define ClearBit(reg, bit) ((reg) &= (~(1<<(bit))))
#define BitIsSet(reg, bit) (((reg) & (1 << (bit))) != 0)
#define Min(a, b) ((a) >= (b) ? (b) : (a))
#define Max(a, b) ((a) >= (b) ? (a) : (b))

#define N 8
#define INPUT "input"
#define OUTPUT "output"

#define BLOCK_SIZE 50000
#define HEADER_LEN 3
#define DICT_SIZE_Q 32769 /*32768 + 1*/
#define LEN_SIZE_Q 259 /*258 + 1*/
#define DICT_MAX 32768
#define LEN_MAX 258
#define EL_SIZE 1 /* element size */
#define MAX_LITLEN_CODE 285
#define MAX_OFF_CODE 29
#define MAX_LITERAL 255
#define CODE_LEN_ALPHABET_SIZE 19 

typedef unsigned char byte;
typedef unsigned short two_bytes;

typedef struct {
	char *input_name;
	char *output_name;
	FILE *input;
	FILE *output;
	long offset;
	size_t size;
} io;

extern FILE *input;
extern FILE *output;

void get_args(int argc, char **argv);
void get_files_name();
void die(char *);
void print_bytes(int, size_t);

#endif
