#ifndef WRITER_H
#define WRITER_H
#include "deflate.h"
#include "cyclic_queue.h"

#define OUTPUT_NAME_LEN 7
typedef struct {
	char *input_name;
	FILE *input;
	FILE *output_file; /* for nocompressed deflate */
	cyclic_queue *output;
	cyclic_queue *cqdict;
	long offset; /* offset in input file */
	size_t block_size; /* block size to write */
	byte write_b; /* byte for writing to output file */
	size_t write_i; /* index of bit in write_b byte */
	bool isfinal; /* is final block */
	size_t result; /* result of block compression */
} io;

void write_huffman_code(io *io_s, size_t huff_code, size_t num);
void write_bits(io *io_s, size_t bits, size_t bits_num);
void write_byte(io *io_s, byte b);
void write_bytes(io *io_s, byte b[], size_t size);
void next_bit(io *io_s);
void byte_flush(io *io_s);
void prepare_input_output(io *io_s);
size_t get_output_size(io *io_s);
void init_io(io **io_s);
void delete_io(io **io_s);
void write_to_output(io *io_s, FILE *output);

#endif
