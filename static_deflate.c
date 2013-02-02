#include <assert.h>
#include "static_deflate.h"
#include "deflate.h"
#include "alphabets.h"
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
		
		if (length == 0)
			length = 1;

		count = fread(buff, EL_SIZE, length, input);
		if (count)
			push_back_cyclic_queue(cqbuff, buff, count);
	}
	/* TODO write end-of-block value */
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

static void write_bits(two_bytes bits, size_t bits_num)
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
