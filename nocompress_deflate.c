#include <stdint.h>
#include "nocompress_deflate.h"

void nocompress_deflate(off_t block_size, bool isfinal)
{
	write_nocompress_header(isfinal);
	byte_flush();
	write_length_of_block(block_size);
	write_data(block_size);
}

static void write_nocompress_header(bool isfinal)
{
	byte header = 0;
	/* BFINAL */
	if (isfinal)
		SetBit(header, 0);
	/* BTYPE */
	
	write_bits(header, HEADER_LEN);
}

static void write_length_of_block(off_t block_size)
{
	uint32_t size = block_size;
	fwrite(&size, sizeof(uint32_t), 1, output);
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

static void write_bits(two_bytes bits, size_t bits_num)
{
	int i;
	for (i = 0; i < bits_num; i++) {
		if (BitIsSet(bits, i))
			SetBit(write_b, write_i);
		next_bit();
	}
}

static void next_bit()
{
	write_i++;
	if (write_i >= N) {
		fwrite(&write_b, EL_SIZE, 1, output);
		write_b = 0;
		write_i = 0;
	}
}

static void byte_flush()
{
	if (write_i > 0) {
		fwrite(&write_b, EL_SIZE, 1, output);
		write_b = 0;
		write_i = 0;
	}
}
