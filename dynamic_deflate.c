#include <string.h>
#include "dynamic_deflate.h"
#include "alphabets.h"
#include "huffman_tree.h"


void dynamic_deflate(off_t block_size, bool isfinal)
{
	size_t res_size = block_size + (block_size / 3) * 12;
	two_bytes inter_res[res_size];
	memset(inter_res, 0, res_size * sizeof(two_bytes));

	size_t real_size = LZ77(inter_res, block_size);
	generate_huffman_codes(inter_res, real_size);
}

size_t LZ77(two_bytes inter_res[], size_t block_size)
{
	byte buff[LEN_MAX];
	size_t count = fread(buff, EL_SIZE, Min(LEN_MAX, block_size), input);
	push_back_cyclic_queue(cqbuff, buff, count);

	byte front_b;
	size_t inter_res_i = 0;
	size_t offset, length, last_count;
	while (!isempty_cyclic_queue(cqbuff)) {
		search_cyclic_queue(cqdict, cqbuff, &offset, &length);
		if (offset == 0 && length == 0) {
			front_b = front_cyclic_queue(cqbuff);
			inter_res[inter_res_i] = front_b;
			inter_res_i++;
			move_front_cyclic_queue(cqbuff, cqdict, 1);
		} else {
			inter_res_i = write_pointer(inter_res, inter_res_i, 
										length, offset);
			move_front_cyclic_queue(cqbuff, cqdict, length);
		}

		if (count < block_size) {
			last_count = fread(buff, EL_SIZE, length, input);
			push_back_cyclic_queue(cqbuff, buff, last_count);
			count += last_count;
		}
	}
	return inter_res_i;
}

void generate_huffman_codes(two_bytes inter_res[], size_t real_size)
{
	int j;
	huffman_tree litlen_codes_num[MAX_LITLEN_CODE + 1];
	for (j = 0; j <= MAX_LITLEN_CODE; j++) {
		litlen_codes_num[j].probability = 0;
		litlen_codes_num[j].code = j;
		litlen_codes_num[j].left = NULL;
		litlen_codes_num[j].right = NULL;
	}

	huffman_tree off_codes_num[MAX_OFF_CODE + 1];
	for (j = 0; j <= MAX_OFF_CODE; j++) {
		off_codes_num[j].probability = 0;
		off_codes_num[j].code = j;
		off_codes_num[j].left = NULL;
		off_codes_num[j].right = NULL;
	}

	int i;
	for (i = 0; i < real_size; ) {
		if (inter_res[i] <= 255) {
			litlen_codes_num[inter_res[i]].probability++;
			i++;
		} else {
			litlen_codes_num[inter_res[i++]].probability++;
			if (inter_res[i] > 0)
				i += 2;
			else
				i += 1;

			off_codes_num[inter_res[i++]].probability++;
			if (inter_res[i] > 0)
				i += 2;
			else
				i += 1;
		}
	}

	qsort(litlen_codes_num, MAX_LITLEN_CODE + 1, 
		  sizeof(huffman_tree), compare_huffman_tree);
	qsort(off_codes_num, MAX_OFF_CODE + 1, 
		  sizeof(huffman_tree), compare_huffman_tree);

	for (i = 0; i <= MAX_OFF_CODE; i++)
		printf("%d, ", off_codes_num[i].code);
	printf("\n");
}

size_t write_pointer(two_bytes inter_res[], 
					 size_t i, 
					 size_t length, 
					 size_t offset)
{
	/* write length code */
	two_bytes len_code = get_code_of_length(length);
	inter_res[i++] = len_code;

	/* write extra_bits of length */
	byte len_extra_bits;
	size_t len_bits_num;
	get_extra_bits_of_length(length, len_code, 
							 &len_extra_bits, &len_bits_num);
	if (len_bits_num > 0) {
		inter_res[i++] = len_bits_num;
		inter_res[i++] = len_extra_bits;
	} else
		i++;

	/* write offset code */
	byte off_code = get_code_of_offset(offset);
	inter_res[i++] = off_code;

	/* write extra bits of offset */
	two_bytes off_extra_bits;
	size_t off_bits_num;
	get_extra_bits_of_offset(offset, off_code, 
							 &off_extra_bits, &off_bits_num);
	if (off_bits_num > 0) {
		inter_res[i++] = off_bits_num;
		inter_res[i++] = off_extra_bits;
	} else
		i++;

	return i;
}

void write_len_extra_bits(two_bytes inter_res[], 
						  size_t i, 
						  two_bytes bits)
{
	int j, b = FIRST_LEN_EXTRA_BIT;
	for (j = 0; b <= LAST_LEN_EXTRA_BIT; b++, j++)
		if (BitIsSet(bits, j))
			SetBit(inter_res[i], b);
}

void write_offset(two_bytes inter_res[], 
				  size_t i, 
				  byte bits)
{
	int j, b = LAST_LEN_EXTRA_BIT + 1;
	for (j = 0; b <= LAST1_OFF_BIT; b++, j++)
		if (BitIsSet(bits, j))
			SetBit(inter_res[i], b);

	i++;
	b = FIRST2_OFF_BIT;
	for ( ; b <= LAST2_OFF_BIT; b++, j++)
		if (BitIsSet(bits, j))
			SetBit(inter_res[i], b);
}

void write_off_extra_bits(two_bytes inter_res[], 
						  size_t i, 
						  two_bytes bits)
{
	int j, b = LAST2_OFF_BIT + 1;
	for (j = 0; b <= LAST_OFF_EXTRA_BIT; b++, j++)
	    if (BitIsSet(bits, j))
			SetBit(inter_res[i], b);
}

