#include <stdbool.h>
#include "alphabets.h"


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
