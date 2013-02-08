#include "nocompress_deflate.h"

#include <stdint.h>
#include "writer.h"

/* internal declarations */
static void write_nocompress_header(io *io_s);
static void write_length_of_block(off_t block_size);
static void write_data(off_t block_size);

/* definitions */
void nocompress_deflate(io *io_s)
{
	write_nocompress_header(io_s);
	byte_flush(io_s);
	write_length_of_block(io_s);
	write_data(block_size);
}

static void write_nocompress_header(bool isfinal)
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

static void write_data(off_t block_size)
{
	byte buff[BUFF_SIZE];
	size_t count = 0, last_count, to_read;
	while (count < block_size && !feof(input)) {
		if (block_size - count < BUFF_SIZE)
			to_read = block_size - count;
		else
			to_read = BUFF_SIZE;
		last_count = fread(buff, EL_SIZE, to_read, input);
		fwrite(buff, EL_SIZE, last_count, output);
		count += last_count;
	}
}
