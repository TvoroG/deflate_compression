#include "dynamic_inflate.h"

#include <string.h>

#include "alphabets.h"
#include "writer.h"

/*void build_huffman_codes(huffman_code huff_code[], size_t size);*/
void read_alphabets(reader_t *reader,
					huffman_code litlen_codes[], size_t hlit, 
					huffman_code offset_codes[], size_t hdist, 
					size_t hclen);
void count_number_of_codes(size_t bl_count[], 
						   huffman_code huff_code[],
						   size_t cl_size);
void find_numerical_value_of_smallest_code(size_t next_code[], 
										   size_t bl_count[], 
										   size_t max_bits);
void assign_numerical_values_to_all_codes(size_t next_code[], 
										  huffman_code huff_code[], 
										  size_t size);
size_t get_max_bits(size_t bl_count[], size_t bl_count_size);
void print_massive(size_t mas[], size_t size);
/*void print_huffman_codes(huffman_code huff_code[], size_t size);*/


void dynamic_inflate(reader_t *reader)
{
	size_t hlit = read_HLIT(reader) + 257;
	size_t hdist = read_HDIST(reader) + 1;
	size_t hclen = read_HCLEN(reader) + 4;

	huffman_code litlen_codes[hlit];
	huffman_code offset_codes[hdist];
	read_alphabets(reader, 
				   litlen_codes, hlit, 
				   offset_codes, hdist, 
				   hclen);
	
	io *io_s;
	init_io(&io_s);
	byte bytes[LEN_MAX];
	size_t litlen;
	bool is_end_of_block = false;
	while (!is_end_of_block) {
		litlen = read_next_huffman_code(reader, litlen_codes, hlit);
		if (litlen < END_OF_BLOCK)
			write_byte(io_s, litlen);
		else if (litlen == END_OF_BLOCK)
			is_end_of_block = true;
		else {
			two_bytes length = decode_length(reader, litlen);
			two_bytes offcode = read_next_huffman_code(reader, 
													   offset_codes, 
													   hdist);
		    two_bytes distance = decode_distance(reader, offcode);
			get_cyclic_queue(io_s->output, bytes, length, distance);
			write_bytes(io_s, bytes, length);
		}
	}

	write_to_output(io_s, reader->output);
	delete_io(&io_s);
}

void read_alphabets(reader_t *reader, 
					huffman_code litlen_codes[], size_t hlit, 
					huffman_code offset_codes[], size_t hdist, 
					size_t hclen)
{
	/* read code length for the code length alphabet */
	size_t cl_len = CODE_LEN_ALPHABET_SIZE;
	huffman_code cl_for_cl[cl_len];
	memset(cl_for_cl, 0, sizeof(huffman_code) * cl_len);
	read_code_length_for_code_length(reader, cl_for_cl, hclen);
	build_huffman_codes(cl_for_cl, cl_len);

	/* read litlen alphabet */
	memset(litlen_codes, 0, sizeof(huffman_code) * hlit);
	read_code_length_for_alphabet(reader, 
								  litlen_codes, hlit, 
								  cl_for_cl, cl_len);
	build_huffman_codes(litlen_codes, hlit);

	/* read offset alphabet */
	memset(offset_codes, 0, sizeof(huffman_code) * hdist);
	read_code_length_for_alphabet(reader, 
								  offset_codes, hdist, 
								  cl_for_cl, cl_len);
	build_huffman_codes(offset_codes, hdist);
}

void build_huffman_codes(huffman_code huff_code[], size_t size)
{
	size_t bl_count_size = sizeof(two_bytes) * N + 1;
	size_t bl_count[bl_count_size];
	memset(bl_count, 0, sizeof(size_t) * bl_count_size);

	count_number_of_codes(bl_count, huff_code, size);

	size_t max_bits = get_max_bits(bl_count, bl_count_size);
	size_t next_code[max_bits + 1];
	find_numerical_value_of_smallest_code(next_code, bl_count,max_bits);
	assign_numerical_values_to_all_codes(next_code, huff_code, size);
}


void count_number_of_codes(size_t bl_count[], 
						   huffman_code huff_code[],
						   size_t cl_size)
{
	int i;
	for (i = 0; i < cl_size; i++)
		if (huff_code[i].code_len != 0)
			bl_count[huff_code[i].code_len]++;
}

void find_numerical_value_of_smallest_code(size_t next_code[], 
										   size_t bl_count[], 
										   size_t max_bits)
{
	size_t code = 0;
	bl_count[0] = 0;
	size_t bits;
	for (bits = 1; bits <= max_bits; bits++) {
		code = (code + bl_count[bits - 1]) << 1;
		next_code[bits] = code;
	}
}

void assign_numerical_values_to_all_codes(size_t next_code[], 
										  huffman_code huff_code[],
										  size_t size)
{
	size_t i;
	for (i = 0; i < size; i++) {
		size_t len = huff_code[i].code_len;
		if (len != 0) {
			huff_code[i].code = next_code[len];
			next_code[len]++;
		}
	}
}

size_t get_max_bits(size_t bl_count[], size_t bl_count_size)
{
	size_t i;
	for (i = bl_count_size - 1; i > 0; i--) {
		if (bl_count[i] > 0)
			break;
	}
	return i;
}

void print_huffman_codes(huffman_code huff_code[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++) {
		printf("%d: code = ", i);
		print_bytes(huff_code[i].code, sizeof(size_t));
		printf(", code_len = %d\n", huff_code[i].code_len);
	}
}

void print_massive(size_t mas[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++) {
		printf("%d: %d\n", i, mas[i]);
	}
}
