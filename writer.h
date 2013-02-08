#ifndef WRITER_H
#define WRITER_H
#include "deflate.h"

#define OUTPUT_NAME_LEN 7
typedef struct {
	char *input_name;
	char output_name[OUTPUT_NAME_LEN];
	FILE *input;
	FILE *output;
	long offset; /* offset in input file */
	size_t block_size; /* block size to write */
	byte write_b; /* byte for writing to output file */
	size_t write_i; /* index of bit in write_b byte */
	bool isfinal; /* is final block */
	size_t result; /* result of block compression */
} io;

void write_huffman_code(io *io_s, size_t huff_code, size_t num);
void write_bits(io *io_s, size_t bits, size_t bits_num);
void next_bit(io *io_s);
void byte_flush(io *io_s);
void prepare_input_file(io *io_s);
size_t get_output_size(io *io_s);
void init_io(io *io_s);

#endif
