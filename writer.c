#include "writer.h"
#include "alphabets.h"

static byte write_b = 0; /* byte for writing to output file */
static size_t write_i = 0; /* index of bit in write_b byte */

void write_huffman_code(size_t huff_code, size_t num)
{
	int i;
	for (i = num - 1; i >= 0; i--) {
		if (BitIsSet(huff_code, i))
			SetBit(write_b, write_i);
		next_bit();
	}
}

void write_bits(size_t bits, size_t bits_num)
{
	int i;
	for (i = 0; i < bits_num; i++) {
		if (BitIsSet(bits, i))
			SetBit(write_b, write_i);
		next_bit();
	}
}

void next_bit()
{
	write_i++;
	if (write_i >= N) {
		fwrite(&write_b, EL_SIZE, 1, output);
		write_b = 0;
		write_i = 0;
	}
}

void byte_flush()
{
	if (write_i > 0) {
		fwrite(&write_b, EL_SIZE, 1, output);
		write_b = 0;
		write_i = 0;
	}
}
