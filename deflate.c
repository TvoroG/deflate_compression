#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "deflate.h"
#include "static_deflate.h"
#include "cyclic_queue.h"

FILE *input;
FILE *output;
byte write_b = 0; /* byte for writing to output file */
size_t write_i = 0; /* index of bit in write_b byte */
cyclic_queue *cqdict; /* dictionary */
cyclic_queue *cqbuff; /* proactive buffer */
static char *ext = ".mz";
static char *usage = "usage: deflate [-d] intput [output]";

struct globalArgs_t {
	char *input_name;
	char *output_name;
	bool isdecompress;
} global_args;

int main(int argc, char **argv)
{
	get_args(argc, argv);
	get_files_name();
	
	input = fopen(global_args.input_name, "r");
	output = fopen(global_args.output_name, "w");
	if (input == NULL || output == NULL)
		die(NULL);

	cqdict = new_cyclic_queue(DICT_SIZE_Q);
	cqbuff = new_cyclic_queue(LEN_SIZE_Q);

	struct stat input_stat;
	stat(INPUT, &input_stat);

	if (global_args.isdecompress) {
		die("decompressor is not implemented yet");
	} else {
		dynamic_deflate(input_stat.st_size, true);
		/*static_deflate(true);*/
	}

	delete_cyclic_queue(cqbuff);
	delete_cyclic_queue(cqdict);

	free(global_args.output_name);
	fclose(input);
	fclose(output);
	exit(0);
}

void get_args(int argc, char **argv)
{	
	global_args.isdecompress = false;
	global_args.input_name = NULL;
	global_args.output_name = NULL;

	char *opt_string = "d";
	int opt = 0;
	opt = getopt(argc, argv, opt_string);
	while (opt != -1) {
		if (opt == 'd') {
			global_args.isdecompress = true;
		}
		opt = getopt(argc, argv, opt_string);
	}
	
	int opt_index = optind;
	while (opt_index < argc) {
		if (global_args.input_name == NULL)
			global_args.input_name = argv[opt_index];
		else if (global_args.output_name == NULL)
			global_args.output_name = argv[opt_index];
		else
			die(usage);
		opt_index++;
	}
}

void get_files_name()
{
	if (global_args.input_name == NULL)
		die(usage);

	if (global_args.output_name == NULL) {
		global_args.output_name = malloc(strlen(global_args.input_name) + strlen(ext) + 1);
		strcpy(global_args.output_name, global_args.input_name);
		strcat(global_args.output_name, ext);
	} else {
		char *output_name = global_args.output_name;
		global_args.output_name = malloc(strlen(global_args.output_name) + strlen(ext) + 1);
		strcpy(global_args.output_name, output_name);
		strcat(global_args.output_name, ext);
	}
}

void die(char *mes)
{
	if (mes != NULL)
		puts(mes);
	else
		perror("error");
	exit(1);
}

void print_bytes(int b, size_t size)
{
	int i;
	for (i = size * N - 1; i >= 0; i--)
		printf("%d", GetBit(b, i));
}
