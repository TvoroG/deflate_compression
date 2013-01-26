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

	static_deflate(input, output);
	fclose(input);
	fclose(output);
	exit(0);
}

void static_deflate(FILE *input, FILE *output)
{
	cyclic_queue *cqdict = new_cyclic_queue(DICT_SIZE_Q);
	cyclic_queue *cqbuff = new_cyclic_queue(LEN_SIZE_Q);

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

	free(buff);
	delete_cyclic_queue(cqbuff);
	delete_cyclic_queue(cqdict);
}

void write_literal(FILE *output, byte lit)
{
	/* get Huffman code */
	int i;
	for (i = N - 1; i >= 0; i--, write_i++) {
		if (BitIsSet(lit, i))
			SetBit(write_b, write_i);
		if (write_i >= N) {
			fwrite(&write_b, EL_SIZE, 1, output);
			write_b = write_i = 0;
		}
	}
}

void write_pointer(FILE *output, size_t length, size_t offset)
{
	/* get Huffman codes for length and offset*/
	write_literal(output, 0);
	write_literal(output, 0);
}

void die(char *mes)
{
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

