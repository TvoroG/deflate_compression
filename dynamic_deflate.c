#include "dynamic_deflate.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

#include "deflate.h"
#include "huffman_tree.h"
#include "alphabets.h"
#include "writer.h"

/* internal declarations */
/* compression functions */
static size_t LZ77(two_bytes inter_res[], io *io_s);
static void init_tree(huffman_tree *tree[], size_t size);
static void count_probability_for_litlen(huffman_tree *tree[], 
										 two_bytes inter_res[], 
										 size_t real_size, 
										 size_t *tree_size,
										 two_bytes *max_code);
static void count_probability_for_offset(huffman_tree *tree[], 
										 two_bytes inter_res[], 
										 size_t real_size, 
										 size_t *tree_size, 
										 two_bytes *max_code);
static void count_probability_for_code_length(huffman_tree *litlen_tree[],
											  size_t max_litlen_code,
											  huffman_tree *off_tree[],
											  size_t max_off_code,
											  huffman_tree *length_tree[],
											  size_t *length_size);
static size_t generate_huffman_codes(huffman_tree *tree[],
									 size_t array_size,
									 size_t tree_size);

static void build_tree(huffman_tree *[], size_t);
static void destroy_tree(huffman_tree *tree[], huffman_tree *cur);
static void find_numerical_values(size_t bl_count[], size_t size, 
								  size_t next_code[]);
static void assign_numerical_values(huffman_tree *tree[], 
									size_t array_size,
									size_t next_code[]);
static size_t get_max_len(size_t bl_count[], size_t size);
static void sort_in_tree(huffman_tree *codes_num[], 
						 huffman_tree *tree, int i);

/* writing functions */
static void write_dynamic_header(io *io_s);
static void write_HLIT_HDIST(io *io_s, two_bytes max_litlen_code, 
							 two_bytes max_off_code);
static size_t write_HCLEN(io *io_s, huffman_tree *length_tree[]);
static void write_code_length_for_code_length(io *io_s, 
											  huffman_tree *length_tree[],
											  size_t HCLEN);
static void write_code_length_for_alphabet(io *io_s, 
										   huffman_tree *tree[],
										   huffman_tree *length_tree[],
										   size_t max_code);
static void write_compressed_data(io *io_s, 
								  two_bytes inter_res[], 
								  size_t real_size, 
								  huffman_tree *litlen_tree[], 
								  huffman_tree *off_tree[]);
static size_t write_pointer(two_bytes inter_res[], 
							size_t i, 
							size_t length, 
							size_t offset);


/* definitions */

void *dynamic_deflate(void *io_struct)
{
	io *io_s = (io *) io_struct;
	prepare_input_file(io_s);

	size_t res_size = io_s->block_size + (io_s->block_size / 3) * 12;
	two_bytes *inter_res = (two_bytes *) malloc(res_size);
	memset(inter_res, 0, res_size * sizeof(two_bytes));

	size_t real_size = LZ77(inter_res, io_s);

	huffman_tree *litlen_tree[MAX_LITLEN_CODE + 1];
	huffman_tree *off_tree[MAX_OFF_CODE + 1];
	init_tree(litlen_tree, MAX_LITLEN_CODE + 1);
	init_tree(off_tree, MAX_OFF_CODE + 1);
	
	size_t litlen_size, off_size;
	two_bytes max_litlen_code, max_off_code;
	count_probability_for_litlen(litlen_tree, 
								 inter_res, 
								 real_size,
								 &litlen_size, 
								 &max_litlen_code);
	count_probability_for_offset(off_tree, 
								 inter_res, 
								 real_size, 
								 &off_size, 
								 &max_off_code);

	size_t max_litlen_len, max_off_len;
	max_litlen_len = generate_huffman_codes(litlen_tree, 
											MAX_LITLEN_CODE + 1, 
											litlen_size);

	max_off_len = generate_huffman_codes(off_tree, 
										 MAX_OFF_CODE + 1, 
										 off_size);

	/* length tree*/
	huffman_tree *length_tree[CODE_LEN_ALPHABET_SIZE];
	init_tree(length_tree, CODE_LEN_ALPHABET_SIZE);

	size_t length_size;
	count_probability_for_code_length(litlen_tree,
									  max_litlen_code,
									  off_tree, 
									  max_off_code,
									  length_tree, 
									  &length_size);

	generate_huffman_codes(length_tree, 
						   CODE_LEN_ALPHABET_SIZE, 
						   length_size);

	/* writing part */
	write_dynamic_header(io_s);
	write_HLIT_HDIST(io_s, max_litlen_code, max_off_code);
	size_t HCLEN = write_HCLEN(io_s, length_tree);

	write_code_length_for_code_length(io_s, length_tree, HCLEN);
	write_code_length_for_alphabet(io_s, litlen_tree, 
								   length_tree, 
								   max_litlen_code);
	write_code_length_for_alphabet(io_s, off_tree,
								   length_tree, 
								   max_off_code);
	write_compressed_data(io_s, inter_res, real_size, 
						  litlen_tree, off_tree);
	if (io_s->isfinal)
		byte_flush(io_s);

	free(inter_res);
	free(io_s->input);

	io_s->result = get_output_size(io_s);
	pthread_exit(NULL);
}

static size_t LZ77(two_bytes inter_res[], io *io_s)
{
	byte buff[LEN_MAX];
	cyclic_queue *cqbuff = new_cyclic_queue(LEN_SIZE_Q);
	size_t count = fread(buff, EL_SIZE, 
						 Min(LEN_MAX, io_s->block_size), io_s->input);
	push_back_cyclic_queue(cqbuff, buff, count);

	byte front_b;
	size_t inter_res_i = 0;
	size_t offset, length, last_count;
	while (!isempty_cyclic_queue(cqbuff)) {
		search_cyclic_queue(io_s->cqdict, cqbuff, &offset, &length);
		if (offset == 0 && length == 0) {
			front_b = front_cyclic_queue(cqbuff);
			inter_res[inter_res_i] = front_b;
			inter_res_i++;
			move_front_cyclic_queue(cqbuff, io_s->cqdict, 1);
		} else {
			inter_res_i = write_pointer(inter_res, inter_res_i, 
										length, offset);
			move_front_cyclic_queue(cqbuff, io_s->cqdict, length);
		}

		if (length == 0)
			length = 1;

		if (count < io_s->block_size) {
			if (count + length <= io_s->block_size)
				length = count + length - io_s->block_size;
			last_count = fread(buff, EL_SIZE, length, io_s->input);
			push_back_cyclic_queue(cqbuff, buff, last_count);
			count += last_count;
		}
	}

	delete_cyclic_queue(cqbuff);
	inter_res[inter_res_i] = END_OF_BLOCK;
	return inter_res_i + 1;
}

static void init_tree(huffman_tree *tree[], size_t size)
{
	int j;
	for (j = 0; j < size; j++)
		tree[j] = new_huffman_tree(0, j, NULL, NULL);
}

void count_probability_for_litlen(huffman_tree *tree[], 
								  two_bytes inter_res[], 
								  size_t real_size, 
								  size_t *tree_size,
								  two_bytes *max_code)
{
	size_t i;
	*max_code = 0;
	*tree_size = 0;
	for (i = 0; i < real_size; ) {
		if (inter_res[i] <= MAX_LITERAL) {
			if (tree[inter_res[i]]->probability == 0)
				(*tree_size)++;
			*max_code = Max(*max_code, inter_res[i]);
			tree[inter_res[i]]->probability++;
			i++;
		} else {
			if (tree[inter_res[i]]->probability == 0)
				(*tree_size)++;
			*max_code = Max(*max_code, inter_res[i]);
			tree[inter_res[i]]->probability++;
			i++;
			/* read extra bits len of length*/
			if (inter_res[i] > 0)
				i += 3;
			else
				i += 2;

			/* read extra bits len of offset*/
			if (inter_res[i] > 0)
				i += 2;
			else
				i += 1;
		}
	}
}

static void count_probability_for_offset(huffman_tree *tree[], 
										 two_bytes inter_res[], 
										 size_t real_size, 
										 size_t *tree_size, 
										 two_bytes *max_code)
{
	size_t i;
	*max_code = 0;
	*tree_size = 0;
	for (i = 0; i < real_size; ) {
		if (inter_res[i] <= MAX_LITERAL) {
			i++;
		} else {
			/* not read lenght code */
			i++;
			/* read extra bits len of length*/
			if (inter_res[i] > 0)
				i += 2;
			else
				i += 1;

			if (tree[inter_res[i]]->probability == 0)
				(*tree_size)++;
			*max_code = Max(*max_code, inter_res[i]);
			tree[inter_res[i]]->probability++;
			i++;
			/* read extra bits len of offset*/
			if (inter_res[i] > 0)
				i += 2;
			else
				i += 1;
		}
	}
}

static void count_probability_for_code_length(huffman_tree *litlen_tree[],
											  size_t max_litlen_code,
											  huffman_tree *off_tree[],
											  size_t max_off_code,
											  huffman_tree *length_tree[], 
											  size_t *length_size)
{
	*length_size = 0;
	huffman_tree **tree;
	size_t i, j, len, num, size;
	j = 0;
	while (j < 2) {
		if (j == 0) {
			tree = litlen_tree;
			size = max_litlen_code;
		} else {
			tree = off_tree;
			size = max_off_code;
		}

		for (i = 0; i <= size; ) {
			len = tree[i]->len;
			if (len != 0 && tree[i]->code == i) {
				if (length_tree[len]->probability == 0)
					(*length_size)++;
				length_tree[len]->probability++;				
				i++;
			} else {
				i++;
				num = 1;
				while (i <= size) {
					len = tree[i]->len;
					if (len != 0 && tree[i]->code == i)
						break;
					else {
						num++;
						i++;
					}
				}

				if (num < REPEAT_17_BEG) {
					i = i - num + 1;
					if (length_tree[0]->probability == 0)
						(*length_size)++;
					length_tree[0]->probability++;
				} else if(num >= REPEAT_17_BEG && num <= REPEAT_17_END) {
					if (length_tree[17]->probability == 0)
						(*length_size)++;
					length_tree[17]->probability++;
				} else if(num >= REPEAT_18_BEG && num <= REPEAT_18_END) {
					if (length_tree[18]->probability == 0)
						(*length_size)++;
					length_tree[18]->probability++;
				} else {
					if (length_tree[18]->probability == 0)
						(*length_size)++;
					length_tree[18]->probability++;
					i = i - (num - REPEAT_18_END);
				}
			}
		}
		j++;
	}
}

static size_t generate_huffman_codes(huffman_tree *tree[],
							  size_t array_size,
							  size_t tree_size)
{
	qsort(tree, array_size, sizeof(huffman_tree *),compare_huffman_tree);

	size_t bl_count[tree_size + 1];
	memset(bl_count, 0, sizeof(size_t) * (tree_size + 1));

	build_tree(tree, tree_size);
	if (tree_size == 1)
		count_len_huffman_tree(tree[0], bl_count, 1);
	else
		count_len_huffman_tree(tree[0], bl_count, 0);
	destroy_tree(tree, tree[0]);
	
	size_t max_len = get_max_len(bl_count, tree_size + 1);

	/* next_code size may be max_len + 1 */
	size_t next_code[tree_size + 1];
	find_numerical_values(bl_count, max_len, next_code);
	
	/* array_size may be replace with max_tree_code*/
	assign_numerical_values(tree, array_size, next_code);
	return max_len;
}

static void write_dynamic_header(io *io_s)
{
	byte header = 0;
	/* BFINAL */
	if (io_s->isfinal)
		SetBit(header, 0);
	/* BTYPE */
	SetBit(header, 1);
	
	write_bits(io_s, header, HEADER_LEN);
}

static void write_HLIT_HDIST(io *io_s, two_bytes max_litlen_code, 
							 two_bytes max_off_code)
{
	write_bits(io_s, max_litlen_code - 257, 5);
	write_bits(io_s, max_off_code - 1, 5);
}

static size_t write_HCLEN(io *io_s, huffman_tree *length_tree[])
{	
	int i, num = CODE_LENGTH_ORDER_SIZE - 1;
	for ( ; num >= 0; num--) {
		i = code_length_order[num];
		if (length_tree[i]->len != 0)
			break;
	}
	size_t HCLEN = num + 1;
	write_bits(io_s, HCLEN - 4, 4);
	return HCLEN;
}

static void write_code_length_for_code_length(io *io_s, 
											  huffman_tree *length_tree[], 
											  size_t HCLEN)
{
	int i;
	size_t len;
	for (i = 0; i < HCLEN; i++) {
		len = code_length_order[i];
		write_bits(io_s, length_tree[len]->len, 3);
	}
}

static void write_code_length_for_alphabet(io *io_s, 
										   huffman_tree *tree[],
										   huffman_tree *length_tree[],
										   size_t max_code)
{
	size_t i, len, num;
	for (i = 0; i <= max_code; ) {
		len = tree[i]->len;
		if (len != 0 && tree[i]->code == i) {
			write_huffman_code(io_s, length_tree[len]->huff_code, 
							   length_tree[len]->len);
			i++;
		} else {
			i++;
			num = 1;
			while (i <= max_code) {
				len = tree[i]->len;
				if (len != 0 && tree[i]->code == i)
					break;
				else {
					num++;
					i++;
				}
			}
			
			if (num < REPEAT_17_BEG) {
				write_huffman_code(io_s, length_tree[0]->huff_code,
								   length_tree[0]->len);
				i = i - num + 1;
			} else if (num >= REPEAT_17_BEG && num <= REPEAT_17_END) {
				write_huffman_code(io_s, length_tree[17]->huff_code,
								   length_tree[17]->len);
				write_bits(io_s, num - REPEAT_17_BEG, 3);
			} else if (num >= REPEAT_18_BEG && num <= REPEAT_18_END) {
				write_huffman_code(io_s, length_tree[18]->huff_code,
								   length_tree[18]->len);
				write_bits(io_s, num - REPEAT_18_BEG, 7);
			} else {
				write_huffman_code(io_s, length_tree[18]->huff_code,
								   length_tree[18]->len);
				write_bits(io_s, REPEAT_18_END - REPEAT_18_BEG, 7);
				i = i - (num - REPEAT_18_END);
			}			
		}
	}
}


static void write_compressed_data(io *io_s, 
								  two_bytes inter_res[], 
								  size_t real_size, 
								  huffman_tree *litlen_tree[], 
								  huffman_tree *off_tree[])
{
	size_t i;
	for (i = 0; i < real_size; ) {
		if (inter_res[i] <= MAX_LITERAL) {
			write_huffman_code(io_s, 
							   litlen_tree[inter_res[i]]->huff_code, 
							   litlen_tree[inter_res[i]]->len);
			i++;
		} else {
			write_huffman_code(io_s, 
							   litlen_tree[inter_res[i]]->huff_code, 
							   litlen_tree[inter_res[i]]->len);
			i++;
			if (inter_res[i] > 0) {
				write_bits(io_s, inter_res[i + 1], inter_res[i]);
				i += 2;
			} else {
				i += 1;
			}

			write_huffman_code(io_s, off_tree[inter_res[i]]->huff_code, 
							   off_tree[inter_res[i]]->len);
			i++;
			if (inter_res[i] > 0) {
				write_bits(io_s, inter_res[i + 1], inter_res[i]);
				i += 2;
			} else {
				i += 1;
			}
		}
	}
}

static size_t write_pointer(two_bytes inter_res[], 
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

static void build_tree(huffman_tree *codes_num[], size_t size)
{
	int i;
	prob_t prob;
	huffman_tree *tree;
	for (i = size - 1; i > 0; i--) {
		prob = codes_num[i]->probability + codes_num[i - 1]->probability;
		tree = new_huffman_tree(prob, -1,
								codes_num[i - 1], 
								codes_num[i]);
		sort_in_tree(codes_num, tree, i);
	}
}

static void destroy_tree(huffman_tree *tree[], huffman_tree *cur)
{
	if (cur->left == NULL && cur->right == NULL) {
		tree[cur->code] = cur;
	} else {
		destroy_tree(tree, cur->left);
		destroy_tree(tree, cur->right);
	}
}

static void find_numerical_values(size_t bl_count[], size_t size, 
								  size_t next_code[])
{
	size_t bits, code = 0;
    bl_count[0] = 0;
    for (bits = 1; bits <= size; bits++) {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }
}

static void assign_numerical_values(huffman_tree *tree[], 
									size_t array_size, 
									size_t next_code[])
{
	size_t i, len;
	for (i = 0; i < array_size; i++) {
		len = tree[i]->len;
		if (len != 0 && tree[i]->code == i) {
			tree[i]->huff_code = next_code[len];
			next_code[len]++;
		}
	}
}

static size_t get_max_len(size_t bl_count[], size_t size)
{
	size_t i;
	for (i = size - 1; i > 0; i--) {
		if (bl_count[i] > 0)
			break;
	}
	return i;
}

static void sort_in_tree(huffman_tree *codes_num[], 
						 huffman_tree *tree, int i)
{
	int j = i - 2;
	while (j >= 0 && 
		   codes_num[j]->probability < tree->probability) {
		codes_num[j + 1] = codes_num[j];
		j--;
	}
	codes_num[j + 1] = tree;
}
