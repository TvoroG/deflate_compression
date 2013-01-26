#include <stdio.h>
#include <stdlib.h>
#include "deflate.h"
#include "cyclic_queue.h"

static byte write_b = 0;
static size_t write_i = 0;

int main()
{
	FILE *input = fopen(INPUT, "r");
	FILE *output = fopen(OUTPUT, "w");
	if (input == NULL || output == NULL)
		die(NULL);

	static_deflate(input, output, true);
	fclose(input);
	fclose(output);
	exit(0);
}

void static_deflate(FILE *input, FILE *output, bool isfinal)
{
	assert(input != NULL && output != NULL);
	cyclic_queue *cqdict = new_cyclic_queue(DICT_SIZE_Q);
	cyclic_queue *cqbuff = new_cyclic_queue(LEN_SIZE_Q);

	write_static_header(output, isfinal);

	byte *buff = (byte *) malloc(LEN_MAX);
	size_t count = fread(buff, EL_SIZE, LEN_MAX, input);
	push_back_cyclic_queue(cqbuff, buff, count);

	byte front_b;
	size_t offset, length;
	while (!isempty_cyclic_queue(cqbuff)) {
		search_cyclic_queue(cqdict, cqbuff, &offset, &length);
		if (offset == 0 && length == 0) {
			front_b = front_cyclic_queue(cqbuff);
			write_literal(output, front_b);
			move_front_cyclic_queue(cqbuff, cqdict, 1);
		} else {
			write_pointer(output, length, offset);
			move_front_cyclic_queue(cqbuff, cqdict, length);
		}
		
		count = fread(buff, EL_SIZE, length, input);
		if (count)
			push_back_cyclic_queue(cqbuff, buff, count);
	}
	if (isfinal)
		byte_flush(output);

	free(buff);
	delete_cyclic_queue(cqbuff);
	delete_cyclic_queue(cqdict);
}

void write_literal(FILE *output, byte lit)
{
	/* get Huffman code */
	int i;
	for (i = N - 1; i >= 0; i--) {
		if (BitIsSet(lit, i))
			SetBit(write_b, write_i);
		next_bit(output);
	}
}

void write_pointer(FILE *output, size_t length, size_t offset)
{
	/* get Huffman codes for length and offset*/
	write_literal(output, 1);
	write_literal(output, 1);
}

void write_static_header(FILE *output, bool isfinal)
{
	/* BFINAL */
	if (isfinal)
		SetBit(write_b, write_i);
	next_bit(output);
	
	/* BTYPE
	01 - compressed with fixed Huffman codes */
	SetBit(write_b, write_i);
	next_bit(output);
	next_bit(output);
}

void next_bit(FILE *output)
{
	write_i++;
	if (write_i >= N) {
		fwrite(&write_b, EL_SIZE, 1, output);
		write_b = 0;
		write_i = 0;
	}
}

void byte_flush(FILE *output)
{
	if (write_i > 0) {
		fwrite(&write_b, EL_SIZE, 1, output);
		write_b = 0;
		write_i = 0;
	}
}

void get_huffman_code_for_litlen_alphabet(two_bytes literal, 
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

void die(char *mes)
{
	if (mes != NULL)
		puts(mes);
	perror("error");
	exit(1);
}

void print_bytes(int b, size_t size)
{
	int i;
	for (i = size * N - 1; i >= 0; i--)
		printf("%d", GetBit(b, i));
	printf("\n");
}

