#include <string.h>
#include "dynamic_deflate.h"
#include "alphabets.h"

void dynamic_deflate(off_t block_size, bool isfinal)
{
	size_t res_size = block_size + (block_size / 3) * 12;
	two_bytes inter_res[res_size];
	memset(inter_res, 0, res_size * sizeof(two_bytes));

	size_t real_size = LZ77(inter_res, block_size);

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


	generate_huffman_codes(litlen_tree, 
						   MAX_LITLEN_CODE + 1, 
						   litlen_size);
	generate_huffman_codes(off_tree, 
						   MAX_OFF_CODE + 1, 
						   off_size);

	print_huffman_tree(litlen_tree[0]);
	printf("\n");

	destroy_tree(litlen_tree, litlen_tree[0]);
	destroy_tree(off_tree, off_tree[0]);
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
			tree[inter_res[i++]]->probability++;
		} else {
			if (tree[inter_res[i]]->probability == 0)
				(*tree_size)++;
			*max_code = Max(*max_code, inter_res[i]);
			tree[inter_res[i++]]->probability++;
			
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

void count_probability_for_offset(huffman_tree *tree[], 
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
			tree[inter_res[i++]]->probability++;

			/* read extra bits len of offset*/
			if (inter_res[i] > 0)
				i += 2;
			else
				i += 1;
		}
	}
}

void generate_huffman_codes(huffman_tree *tree[],
							size_t array_size,
							size_t tree_size)
{
	qsort(tree, array_size, sizeof(huffman_tree *),compare_huffman_tree);
	
	size_t bl_count[tree_size + 1];
	memset(bl_count, 0, sizeof(size_t) * (tree_size + 1));

	build_tree(tree, tree_size + 1);
	if (tree_size == 1)
		count_len_huffman_tree(tree[0], bl_count, 1);
	else
		count_len_huffman_tree(tree[0], bl_count, 0);

	size_t next_code[tree_size + 1];
	find_numerical_values(bl_count, tree_size + 1, next_code);
	assign_numerical_values(tree[0], next_code);
}

void init_tree(huffman_tree *tree[], size_t size)
{
	int j;
	for (j = 0; j < size; j++)
		tree[j] = new_huffman_tree(0, j, NULL, NULL);
}

void build_tree(huffman_tree *codes_num[], size_t size)
{
	int i;
	prob_t prob;
	huffman_tree *tree;
	for (i = size - 1; i > 0; i--) {
		prob = codes_num[i]->probability + codes_num[i - 1]->probability;
		tree = new_huffman_tree(prob, 0,
								codes_num[i - 1], 
								codes_num[i]);
		sort_in_tree(codes_num, tree, i);
	}
}

void destroy_tree(huffman_tree *tree[], huffman_tree *cur)
{
	if (cur->left == NULL && cur->right == NULL) {
		tree[cur->code] = cur;
	} else {
		destroy_tree(tree, cur->left);
		destroy_tree(tree, cur->right);
	}
}

void sort_in_tree(huffman_tree *codes_num[], huffman_tree *tree, int i)
{
	int j = i - 2;
	while (j >= 0 && 
		   codes_num[j]->probability < tree->probability) {
		codes_num[j + 1] = codes_num[j];
		j--;
	}
	codes_num[j + 1] = tree;
}

void find_numerical_values(size_t bl_count[], 
						   size_t size, 
						   size_t next_code[])
{
	size_t bits, code = 0;
    bl_count[0] = 0;
    for (bits = 1; bits <= size; bits++) {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }
}

void assign_numerical_values(huffman_tree *tree, size_t next_code[])
{
	if (tree->left == NULL && tree->right == NULL) {
		size_t len;
		len = tree->len;
		if (len != 0) {
			tree->huff_code = next_code[len];
			next_code[len]++;
		}
	} else {
		assign_numerical_values(tree->left, next_code);
		assign_numerical_values(tree->right, next_code);
	}
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
