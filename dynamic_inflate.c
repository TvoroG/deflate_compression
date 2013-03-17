#include "dynamic_inflate.h"

#include <string.h>

/*void build_huffman_codes(huffman_code huff_code[], size_t size);*/
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
/*void print_huffman_codes(huffman_code huff_code[], size_t size);*/


void dynamic_inflate(reader_t *reader)
{
	byte hlit = read_HLIT(reader) - 257;
	byte hdist = read_HDIST(reader) - 1;
	byte hclen = read_HCLEN(reader) - 4;

	huffman_code cl_for_cl[hclen];
	read_code_length_for_code_length(reader, cl_for_cl, hclen);
	build_huffman_codes(cl_for_cl, hclen);
	

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
	for (i = bl_count_size - 1; i > 0; i++) {
		if (bl_count[i] > 0)
			break;
	}
	return i;
}

void print_huffman_codes(huffman_code huff_code[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++) {
		printf("code = ");
		print_bytes(huff_code[i].code, sizeof(size_t));
		printf(", code_len = %d\n", huff_code[i].code_len);
	}
}
