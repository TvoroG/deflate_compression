#include "nocompress_deflate.h"
#include <stdint.h>

/* internal declarations */
static void write_nocompress_header(io *io_s);
static void write_length_of_block(io *io_s);

/* definitions */
void nocompress_deflate(io *io_s)
{
	prepare_input_output(io_s);
	write_nocompress_header(io_s);
	byte_flush(io_s);
	write_length_of_block(io_s);
	write_data(io_s);
}

static void write_nocompress_header(io *io_s)
{
	byte header = 0;
	/* BFINAL */
	if (io_s->isfinal)
		SetBit(header, 0);
	/* BTYPE */
	
	write_bits(io_s, header, HEADER_LEN);
}

static void write_length_of_block(io *io_s)
{
	two_bytes len = io_s->block_size;
	two_bytes nlen = ~len;
	fwrite(&len, sizeof(two_bytes), 1, io_s->output_file);
	fwrite(&nlen, sizeof(two_bytes), 1, io_s->output_file);
}
