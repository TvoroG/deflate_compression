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
#define OUTPUT_NAME_LEN 7

typedef unsigned char byte;
typedef unsigned short two_bytes;

struct globalArgs_t {
	char *input_name;
	char *output_name;
	bool isdecompress;
};

typedef struct {
	char *input_name;
	char output_name[OUTPUT_NAME_LEN];
	FILE *input;
	FILE *output;
	long offset; /* offset in input file */
	size_t block_size; /* block size to write */
	byte write_b; /* byte for writing to output file */
	size_t wirte_i; /* index of bit in write_b byte */
	bool isfinal;
} io;

void get_args(int argc, char **argv);
void get_files_name();
void die(char *);
void init_io(io *io_s);
void print_bytes(int, size_t);

#endif
