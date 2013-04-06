#include <stdbool.h>
#include "alphabets.h"

_length_codes_t length_codes[] = {
	0, 3,
	0, 4,
	0, 5,
	0, 6,
	0, 7,
	0, 8,
	0, 9,
	0, 10,
	1, 11,
	1, 13,
	1, 15,
	1, 17,
	2, 19,
	2, 23,
	2, 27,
	2, 31,
	3, 35,
	3, 43,
	3, 51,
	3, 59,
	4, 67,
	4, 83,
	4, 99,
	4, 115,
	5, 131,
	5, 163,
	5, 195,
	5, 227,
	0, 258
};

_offset_codes_t offset_codes[] = {
	0, 1,
	0, 2,
	0, 3,
	0, 4,
	1, 5,
	1, 7,
	2, 9,
	2, 13,
	3, 17,
	3, 25,
	4, 33,
	4, 49,
	5, 65,
	5, 97,
	6, 129,
	6, 193,
	7, 257,
	7, 385,
	8, 513,
	8, 769,
	9, 1025,
	9, 1537,
	10, 2049,
	10, 3073,
	11, 4097,
	11, 6145,
	12, 8193,
	12, 12289,
	13, 16385,
	13, 24577
};

_huffman_codes_t huffman_codes[] = {
	8, 0, 48,
	9, 144, 400,
	7, 256, 0,
	8, 280, 192
};

size_t code_length_order[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

two_bytes get_code_of_length(size_t length)
{
	assert(length >= 3 && length <= 258);
	bool found = false;
	int i;
	two_bytes code = LEN_CODE_BEGINNING;
	for (i = 0; i < LEN_CODE_NUM - 1 && !found; i++, code++) {
		if (length >= length_codes[i].base_len &&
			length <= length_codes[i + 1].base_len - 1)
			found = true;
	}
	/* if not found then return last one */
	return found ? code - 1 : LEN_CODE_BEGINNING + LEN_CODE_NUM - 1;
}

void get_extra_bits_of_length(size_t length,
							  two_bytes len_code,
							  byte *extra_bits, 
							  size_t *bits_num)
{
	int i = len_code - LEN_CODE_BEGINNING;
	*bits_num = length_codes[i].extra_bits_num;

	*extra_bits = length - length_codes[i].base_len;
}

byte get_code_of_offset(size_t offset)
{
	assert(offset >= 1 && offset <= 32768);
	byte i;
	for (i = 0; i < OFF_CODE_NUM - 1; i++) {
		if (offset >= offset_codes[i].base_off &&
			offset <= offset_codes[i + 1].base_off - 1)
			break;
	}
	return i;
}

void get_extra_bits_of_offset(size_t offset, 
							  byte off_code, 
							  two_bytes *extra_bits, 
							  size_t *bits_num)
{
	*bits_num = offset_codes[off_code].extra_bits_num;
	*extra_bits = offset - offset_codes[off_code].base_off;
}


void get_huffman_code_of_litlen(two_bytes literal, 
								two_bytes *code, 
								size_t *code_len)
{
	assert(literal >= 0 && literal <= 287);
	int i;
	for (i = 0; i < HUFF_CODE_NUM - 1; i++) {
		if (literal >= huffman_codes[i].base_lit_value &&
			literal <= huffman_codes[i + 1].base_lit_value - 1) 
			break;
	}
	*code = huffman_codes[i].base_code + 
		literal - huffman_codes[i].base_lit_value;
	*code_len = huffman_codes[i].bits_num;
}
