#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "static_deflate.h"
#include "alphabets.h"
#include "writer.h"

void *static_deflate(void *io_struct)
{
	io *io_s = (io *) io_struct;
	prepare_input_file(io_s);

	write_static_header(io_s);

	byte *buff = (byte *) malloc(LEN_MAX);
	size_t count = fread(buff, EL_SIZE, 
						 Min(LEN_MAX, io_s->block_size), io_s->input);
	push_back_cyclic_queue(cqbuff, buff, count);

	byte front_b;
	size_t offset, length, last_count;
	while (!isempty_cyclic_queue(cqbuff)) {
		search_cyclic_queue(cqdict, cqbuff, &offset, &length);
		if (offset == 0 && length == 0) {
			front_b = front_cyclic_queue(cqbuff);
			write_literal(io_s, front_b);
			move_front_cyclic_queue(cqbuff, cqdict, 1);
		} else {
			write_pointer(io_s, length, offset);
			move_front_cyclic_queue(cqbuff, cqdict, length);
		}
		
		if (length == 0)
			length = 1;

		if (count < io_s->block_size) {
			if (count + length <= io_s->block_size) {
				last_count = fread(buff, EL_SIZE, length, io_s->input);
			} else {
				length = count + length - io_s->block_size;
				last_count = fread(buff, EL_SIZE, length, io_s->input);
			}
			push_back_cyclic_queue(cqbuff, buff, last_count);
			count += last_count;
		}
	}

	write_end_of_block(io_s);
	if (io_s->isfinal)
		byte_flush(io_s);

	free(buff);
	fclose(io_s->input);

	size_t res = get_output_size(io_s);
	pthread_exit(&res);
}

static void write_literal(io *io_s, two_bytes code)
{
	two_bytes huff_code;
	size_t huff_code_len;
	get_huffman_code_of_litlen(code, &huff_code, &huff_code_len);
	write_huffman_code(io_s, huff_code, huff_code_len);
}

static void write_pointer(io *io_s, size_t length, size_t offset)
{
	/* write length huffman code */
	two_bytes huff_code, len_code = get_code_of_length(length);
	size_t huff_code_len;
	get_huffman_code_of_litlen(len_code, &huff_code, &huff_code_len);
	write_huffman_code(io_s, huff_code, huff_code_len);

	/* write extra bits of length */
	byte len_extra_bits;
	size_t len_bits_num;
	get_extra_bits_of_length(length, len_code, 
							 &len_extra_bits, &len_bits_num);
	if (len_bits_num > 0)
		write_bits(io_s, len_extra_bits, len_bits_num);
	
	/* write offset code*/
	byte off_code = get_code_of_offset(offset);
	write_bits(io_s, off_code, OFF_CODE_LEN);
	
	/* write extra bits of offset */
	two_bytes off_extra_bits;
	size_t off_bits_num;
	get_extra_bits_of_offset(offset, off_code, 
							 &off_extra_bits, &off_bits_num);
	if (off_bits_num > 0)
		write_bits(io_s, off_extra_bits, off_bits_num);
}

/* 01 - compressed with fixed Huffman codes */
static void write_static_header(io *io_s)
{
	byte header = 0;
	/* BFINAL */
	if (io_s->isfinal)
		SetBit(header, 0);
	/* BTYPE */
	SetBit(header, 1);
	
	write_bits(io_s, header, HEADER_LEN);
}

static void write_end_of_block(io *io_s)
{	
	two_bytes huff_code, code = END_OF_BLOCK;
	size_t huff_code_len;
	get_huffman_code_of_litlen(code, &huff_code, &huff_code_len);
	write_huffman_code(io_s, huff_code, huff_code_len);
}

static void prepare_input_file(io *io_s)
{
	io_s->input = fopen(io_s->input_name, "r");
	if (fseek(io_s->input, io_s->offset, SEEK_SET))
		die(NULL);
}

static size_t get_output_size(io *io_s)
{
	struct stat output_stat;
	stat(io_s->output_name, &output_stat);
	return output_stat.st_size;
}
