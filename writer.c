#include "writer.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "alphabets.h"

#define WRITE_BUFF_SIZE 1000

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

void write_byte(io *io_s, byte b)
{
	assert(io_s->write_i == 0);
	add_cyclic_queue(io_s->output, b);
}

void write_bytes(io *io_s, byte b[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++) {
		add_cyclic_queue(io_s->output, b[i]);
	}
}

void next_bit(io *io_s)
{
	io_s->write_i++;
	if (io_s->write_i >= N) {
		add_cyclic_queue(io_s->output, io_s->write_b);
		io_s->write_b = 0;
		io_s->write_i = 0;
	}
}

void byte_flush(io *io_s)
{
	if (io_s->write_i > 0) {
		add_cyclic_queue(io_s->output, io_s->write_b);
		io_s->write_b = 0;
		io_s->write_i = 0;
	}
}

void prepare_input_output(io *io_s)
{
	if (io_s->input == NULL)
		io_s->input = fopen(io_s->input_name, "r");
	if (fseek(io_s->input, io_s->offset, SEEK_SET))
		die(NULL);

	clear_cyclic_queue(io_s->output);
}

size_t get_output_size(io *io_s)
{
	return size_cyclic_queue(io_s->output);
}

void init_io(io **io_s)
{
	(*io_s) = (io *) malloc(sizeof(io));
	
	(*io_s)->input = NULL;
	(*io_s)->input_name = global_args.input_name;

	(*io_s)->output = new_cyclic_queue(2 * BLOCK_SIZE);
	(*io_s)->cqdict = new_cyclic_queue(DICT_SIZE_Q);

	(*io_s)->write_b = 0;
	(*io_s)->write_i = 0;
	(*io_s)->isfinal = false;
}

void delete_io(io **io_s)
{
	if ((*io_s)->input != NULL)
		fclose((*io_s)->input);

	delete_cyclic_queue((*io_s)->output);
	delete_cyclic_queue((*io_s)->cqdict);
	free(*io_s);
}

void write_to_output(io *io_s, FILE *output)
{	
	cyclic_queue *input = io_s->output;

	byte buff[WRITE_BUFF_SIZE];
	size_t last_size, size = 0, size_cq = size_cyclic_queue(input);
	while (size < size_cq) {
		last_size = read_cyclic_queue(input, buff, size, WRITE_BUFF_SIZE);
		fwrite(buff, EL_SIZE, last_size, output);
		size += last_size;
	}
}
