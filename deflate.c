#include <stdio.h>
#include <stdlib.h>
#include "deflate.h"
#include "cyclic_queue.h"

int main()
{
	FILE *input = fopen(INPUT, "r");
	FILE *output = fopen(OUTPUT, "w");
	if (input == NULL || output == NULL)
		die(NULL);
	
	static_deflate(input, output);
	exit(0);
}

void static_deflate(FILE *input, FILE *output)
{
	cyclic_queue *cqdict = new_cyclic_queue(DICT_SIZE_Q);
	cyclic_queue *cqbuff = new_cyclic_queue(LEN_SIZE_Q);
	byte rb, wb = 0;
	/* last block */
	SetBit(wb, 1);
	/* fixed Huffman code type */
	SetBit(wb, 2);
	ClearBit(wb, 3);

	fread(&rb, 1, 1, input);
	while (!feof(input)) {
		add_cyclic_queue(cqdict, rb);
		fread(&rb, 1, 1, input);
	}
	print_cyclic_queue(cqdict);
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

