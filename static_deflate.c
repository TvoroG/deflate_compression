#include <assert.h>
#include "static_deflate.h"
#include "deflate.h"
#include "cyclic_queue.h"

void static_deflate(bool isfinal)
{
	write_static_header(isfinal);

	byte *buff = (byte *) malloc(LEN_MAX);
	size_t count = fread(buff, EL_SIZE, LEN_MAX, input);
	push_back_cyclic_queue(cqbuff, buff, count);

	byte front_b;
	size_t offset, length;
	while (!isempty_cyclic_queue(cqbuff)) {
		search_cyclic_queue(cqdict, cqbuff, &offset, &length);
		if (offset == 0 && length == 0) {
			front_b = front_cyclic_queue(cqbuff);
			write_literal(front_b);
			move_front_cyclic_queue(cqbuff, cqdict, 1);
		} else {
			write_pointer(length, offset);
			move_front_cyclic_queue(cqbuff, cqdict, length);
		}
		
		count = fread(buff, EL_SIZE, length, input);
		if (count)
			push_back_cyclic_queue(cqbuff, buff, count);
	}
	if (isfinal)
		byte_flush();

	free(buff);
}

static void write_literal(two_bytes code)
{
	two_bytes huff_code;
	size_t huff_code_len;
	get_huffman_code_of_litlen(code, &huff_code, &huff_code_len);
	write_huffman_code(huff_code, huff_code_len);
}

static void write_pointer(size_t length, size_t offset)
{
	/* write length huffman code */
	two_bytes huff_code, len_code = get_code_of_length(length);
	size_t huff_code_len;
	get_huffman_code_of_litlen(len_code, &huff_code, &huff_code_len);
	write_huffman_code(huff_code, huff_code_len);

	/* write extra bits of length */
	byte len_extra_bits;
	size_t len_bits_num;
	get_extra_bits_of_length(length, len_code, 
							 &len_extra_bits, &len_bits_num);
	if (len_bits_num > 0)
		write_bits(len_extra_bits, len_bits_num);
	
	/* write offset code*/
	byte off_code = get_code_of_offset(offset);
	write_bits(off_code, OFF_CODE_LEN);
	
	/* write extra bits of offset */
	two_bytes off_extra_bits;
	size_t off_bits_num;
	get_extra_bits_of_offset(offset, off_code, 
							 &off_extra_bits, &off_bits_num);
	if (off_bits_num > 0)
		write_bits(off_extra_bits, off_bits_num);
}

/* 01 - compressed with fixed Huffman codes */
static void write_static_header(bool isfinal)
{
	byte header = 0;
	/* BFINAL */
	if (isfinal)
		SetBit(header, 0);
	/* BTYPE */
	SetBit(header, 1);
	
	write_bits(header, HEADER_LEN);
}

static void write_huffman_code(two_bytes huff_code, size_t num)
{
	int i;
	for (i = num - 1; i >= 0; i--) {
		if (BitIsSet(huff_code, i))
			SetBit(write_b, write_i);
		next_bit();
	}
}

static void write_bits(byte bits, size_t bits_num)
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

static void get_huffman_code_of_litlen(two_bytes literal, 
								two_bytes *code, 
								size_t *code_len)
{
	if (literal >= RANGE1_BEGINNING && literal <= RANGE1_END) {
		*code = literal + RANGE1_BASE;
		*code_len = RANGE1_LEN;
	} else if (literal >= RANGE2_BEGINNING && literal <= RANGE2_END) {
		*code = literal - RANGE2_BEGINNING + RANGE2_BASE;
		*code_len = RANGE2_LEN;
	} else if (literal >= RANGE3_BEGINNING && literal <= RANGE3_END) {
		*code = literal - RANGE3_BEGINNING + RANGE3_BASE;
		*code_len = RANGE3_LEN;
	} else if (literal >= RANGE4_BEGINNING && literal <= RANGE4_END) {
		*code = literal  - RANGE4_BEGINNING + RANGE4_BASE;
		*code_len = RANGE4_LEN;
	} else
		die("error in getting huffman code");
}

static two_bytes get_code_of_length(size_t length)
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

static void get_extra_bits_of_length(size_t length,
							  two_bytes len_code,
							  byte *extra_bits, 
							  size_t *bits_num)
{
	int i = len_code - LEN_CODE_BEGINNING;
	*bits_num = length_codes[i].extra_bits_num;

	*extra_bits = length - length_codes[i].base_len;
}

static byte get_code_of_offset(size_t offset)
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

static void get_extra_bits_of_offset(size_t offset, 
							  byte off_code, 
							  two_bytes *extra_bits, 
							  size_t *bits_num)
{
	*bits_num = offset_codes[off_code].extra_bits_num;
	*extra_bits = offset - offset_codes[off_code].base_off;
}



