#include "reader.h"

#include "alphabets.h"

#define CL_FOR_CL_LEN 3

void init_reader(reader_t **reader)
{
	*reader = (reader_t *) malloc(sizeof(reader_t));
	
	(*reader)->input_name = global_args.input_name;
	(*reader)->input = fopen((*reader)->input_name, "r");
	(*reader)->output = fopen(global_args.output_name, "w");	

	read_next_byte(*reader);
	(*reader)->read_i = 0;

	(*reader)->isfinal = false;
	(*reader)->compress_type = ERROR_C;
}

void delete_reader(reader_t **reader)
{
	if ((*reader)->input != NULL)
		fclose((*reader)->input);

	free(*reader);
	*reader = NULL;
}

void read_block_header(reader_t *reader)
{
	if (BitIsSet(reader->read_b, reader->read_i))
		reader->isfinal = true;
	read_next_bit(reader);

	byte b = read_bits(reader, 2);
	if (b == 0)
		reader->compress_type = NO_C;
	else if (b == 1)
		reader->compress_type = STATIC_C;
	else if (b == 2)
		reader->compress_type = DYNAMIC_C;
	else
		reader->compress_type = ERROR_C;
}

two_bytes decode_next_litlen(reader_t *reader)
{
	two_bytes tb = 0;
	two_bytes base_code, last_code;
	int i;

	/* read first seven bits */
	for (i = 6; i >= 0; i--) {
		if (BitIsSet(reader->read_b, reader->read_i))
			SetBit(tb, i);
		read_next_bit(reader);
	}
	if (is_in_huffman_code(tb, 2))
		return tb - huffman_codes[2].base_code + huffman_codes[2].base_lit_value;

	/* if tb does not match then read next bit and compare
	 with codes which length is 8 */
	tb = tb << 1;
	if (BitIsSet(reader->read_b, reader->read_i))
		SetBit(tb, 0);
	read_next_bit(reader);
	if (is_in_huffman_code(tb, 0))
		return tb - huffman_codes[0].base_code + huffman_codes[0].base_lit_value;
	else if (is_in_huffman_code(tb, 3))
		return tb - huffman_codes[3].base_code + huffman_codes[3].base_lit_value;

	/* compare with code which length is 9 */
	tb = tb << 1;
	if (BitIsSet(reader->read_b, reader->read_i))
		SetBit(tb, 0);
	read_next_bit(reader);
	if (is_in_huffman_code(tb, 1))
		return tb - huffman_codes[1].base_code + huffman_codes[1].base_lit_value;

	die("error in decode_next_litlen");
	return tb;
}

two_bytes decode_length(reader_t *reader, two_bytes lencode)
{
	size_t extra_bits_num = length_codes[lencode - LEN_CODE_BEGINNING].extra_bits_num;
	
	size_t extra_len = read_bits(reader, extra_bits_num);
	two_bytes len = length_codes[lencode - LEN_CODE_BEGINNING].base_len + extra_len;
	return len;
}

two_bytes decode_distance(reader_t *reader)
{
	two_bytes offset_code = read_bits(reader, OFF_CODE_LEN);

	size_t extra_bits_num = offset_codes[offset_code].extra_bits_num;
	size_t extra_off = read_bits(reader, extra_bits_num);

	two_bytes off = offset_codes[offset_code].base_off + extra_off;
	return off;
}

byte read_HLIT(reader_t *reader)
{
	byte hlit = read_bits(reader, 5);
	return hlit;
}

byte read_HDIST(reader_t *reader)
{
	byte hdist = read_bits(reader, 5);
	return hdist;
}

byte read_HCLEN(reader_t *reader)
{
	byte hclen = read_bits(reader, 4);
	return hclen;
}

void read_next_bit(reader_t *reader)
{
	reader->read_i++;
	if (reader->read_i >= N) {
		reader->read_i = 0;
		read_next_byte(reader);
	}
}

void read_code_length_for_code_length(reader_t *reader, 
									  huffman_code cl_for_cl[],
									  size_t hclen)
{
	int i;
	for (i = 0; i < hclen; i++) {
		byte len = read_bits(reader, CL_FOR_CL_LEN);
		cl_for_cl[code_length_order[i]].code_len = len;
	}
}

void read_code_length_for_alphabet(reader_t *reader, 
								   huffman_code alphabet_codes[], 
								   size_t alphabet_size, 
								   huffman_code cl_for_cl[], 
								   size_t cl_len)
{
	size_t num;
	for (num = 0; num < alphabet_size; ) {
		size_t cl_i = read_next_huffman_code(reader, cl_for_cl, cl_len);
		if (cl_i == 17) {
			size_t repeat_times = 3 + read_bits(reader, 3);
			num += repeat_times;
		} else if (cl_i == 18) {
			size_t repeat_times = 11 + read_bits(reader, 7);
			num += repeat_times;
		} else {
			alphabet_codes[num].code_len = cl_i;
			num++;
		}
	}
}

size_t read_next_huffman_code(reader_t *reader, 
							  huffman_code cl_for_cl[], 
							  size_t cl_len)
{
	size_t cl_i;
	size_t code_len = 1;
	size_t code = read_bits(reader, 1);
	bool found = false;
	while(!found) {
		size_t clcl;
		for (clcl = 0; clcl < cl_len && !found; clcl++) {
			if (cl_for_cl[clcl].code_len == code_len && 
				cl_for_cl[clcl].code == code) {
				found = true;
				cl_i = clcl;
			}
		}
		if (!found) {
			code_len++;
			code = (code << 1) | read_bits(reader, 1);
		}
	}
	return cl_i;
}

void read_next_byte(reader_t *reader)
{
	byte b;
	fread(&b, sizeof(byte), 1, reader->input);
	reader->read_b = b;
}

bool is_in_huffman_code(two_bytes code, int index)
{
	assert(index >= 0 && index <= HUFF_CODE_NUM);

	two_bytes next_base;
	if (index == HUFF_CODE_NUM - 1)
		next_base = LAST_LIT_VALUE + 1;
	else
		next_base = huffman_codes[index + 1].base_lit_value;

	two_bytes base_code = huffman_codes[index].base_code;
	two_bytes last_code = base_code + next_base - huffman_codes[index].base_lit_value - 1;

	if (code >= base_code && code <= last_code)
		return true;
	else
		return false;
}

size_t read_bits(reader_t *reader, size_t num)
{
	size_t res = 0;
	size_t i;
	for (i = 0; i < num; i++) {
		if (BitIsSet(reader->read_b, reader->read_i))
			SetBit(res, i);
		read_next_bit(reader);
	}
	return res;
}
