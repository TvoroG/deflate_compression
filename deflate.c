#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "deflate.h"
#include "static_deflate.h"
#include "cyclic_queue.h"

FILE *input;
FILE *output;
byte write_b = 0; /* byte for writing to output file */
size_t write_i = 0; /* index of bit in write_b byte */
cyclic_queue *cqdict; /* dictionary */
cyclic_queue *cqbuff; /* proactive buffer */

int main()
{
	input = fopen(INPUT, "r");
	output = fopen(OUTPUT, "w");
	if (input == NULL || output == NULL)
		die(NULL);

	cqdict = new_cyclic_queue(DICT_SIZE_Q);
	cqbuff = new_cyclic_queue(LEN_SIZE_Q);

	struct stat input_stat;
	stat(INPUT, &input_stat);
	dynamic_deflate(input_stat.st_size, true);
/*	static_deflate(true);*/

	delete_cyclic_queue(cqbuff);
	delete_cyclic_queue(cqdict);

	fclose(input);
	fclose(output);
	exit(0);
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
}
