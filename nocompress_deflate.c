#include "nocompress_deflate.h"
#include <stdint.h>

#define BUFF_SIZE 1000

/* internal declarations */
static void write_nocompress_header(io *io_s);
static void write_length_of_block(io *io_s);
static void write_data(io *io_s);

/* definitions */
void nocompress_deflate(io *io_s)
{
	prepare_input_file(io_s);
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
	uint32_t size = io_s->block_size;
	fwrite(&size, sizeof(uint32_t), 1, io_s->output);
}

static void write_data(io *io_s)
{
	byte buff[BUFF_SIZE];
	size_t count = 0, last_count, to_read;
	while (count < io_s->block_size) {
		if (io_s->block_size - count < BUFF_SIZE)
			to_read = io_s->block_size - count;
		else
			to_read = BUFF_SIZE;
		last_count = fread(buff, EL_SIZE, to_read, io_s->input);
		fwrite(buff, EL_SIZE, last_count, io_s->output);
		count += last_count;
	}
}
