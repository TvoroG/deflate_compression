#include "writer.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
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

void prepare_input_file(io *io_s)
{
	io_s->input = fopen(io_s->input_name, "r");
	if (fseek(io_s->input, io_s->offset, SEEK_SET))
		die(NULL);
}

size_t get_output_size(io *io_s)
{
	struct stat output_stat;
	stat(io_s->output_name, &output_stat);
	return output_stat.st_size;
}

void init_io(io *io_s)
{
	io_s->input_name = global_args.input_name;
	io_s->write_b = 0;
	io_s->write_i = 0;
	io_s->isfinal = false;

	strcpy(io_s->output_name, "XXXXXX");
	int fd = mkstemp(io_s->output_name);
	io_s->output = fdopen(fd, "r+w");
}
