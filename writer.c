#include "writer.h"
#include "alphabets.h"

void write_huffman_code(io *io_s, size_t huff_code, size_t num)
{
	int i;
	for (i = num - 1; i >= 0; i--) {
		if (BitIsSet(huff_code, i))
			SetBit(io_s->write_b, io_s->write_i);
		next_bit(io_s);
	}
}

void write_bits(io *io_s, size_t bits, size_t bits_num)
{
	int i;
	for (i = 0; i < bits_num; i++) {
		if (BitIsSet(bits, i))
			SetBit(io_s->write_b, io_s->write_i);
		next_bit(io_s);
	}
}

void next_bit(io *io_s)
{
	io_s->write_i++;
	if (io_s->write_i >= N) {
		fwrite(&io_s->write_b, EL_SIZE, 1, io_s->output);
		io_s->write_b = 0;
		io_s->write_i = 0;
	}
}

void byte_flush(io *io_s)
{
	if (io_s->write_i > 0) {
		fwrite(&io_s->write_b, EL_SIZE, 1, io_s->output);
		io_s->write_b = 0;
		io_s->write_i = 0;
	}
}
