#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "deflate.h"
#include "static_deflate.h"
#include "dynamic_deflate.h"
#include "nocompress_deflate.h"
#include "cyclic_queue.h"
#include "writer.h"

#include "inflate.h"
#include "reader.h"

#include "tests.h"
static void do_tests();

static char *ext = ".mz";
static char *usage = "usage: deflate [-d] intput [output]";
struct globalArgs_t global_args;

int main(int argc, char **argv)
{
	get_args(argc, argv);
	get_files_name();
	
	do_tests();

//	input = fopen(global_args.input_name, "r");
	FILE *output = fopen(global_args.output_name, "w");
	if (output == NULL)
		die(NULL);

	struct stat input_stat;
	stat(global_args.input_name, &input_stat);
	size_t st_size = input_stat.st_size;

	if (global_args.isdecompress) {
		reader_t *reader;
		init_reader(&reader);
		inflate(reader);
		delete_reader(&reader);
	} else {
		int rc1, rc2;
		pthread_t thread_static, thread_dynamic;

		size_t last_size, size = 0;

		void *size_static_p, *size_dynamic_p;
		size_t size_static, size_dynamic;

		io *io_static, *io_dynamic, *io_nocom;
		init_io(&io_static);
		init_io(&io_dynamic);
		init_io(&io_nocom);
		io_nocom->output_file = output;
		while (size < st_size) {
			if (st_size - size >= BLOCK_SIZE) {
				last_size = BLOCK_SIZE;
			} else {
				last_size = st_size - size;
				io_static->isfinal = true;
				io_dynamic->isfinal = true;
				io_nocom->isfinal = true;
			}

			io_static->offset = size;
			io_static->block_size = last_size;
			io_dynamic->offset = size;
			io_dynamic->block_size = last_size;
			io_nocom->offset = size;
			io_nocom->block_size = last_size;
			size += last_size;

			rc1 = pthread_create(&thread_static, NULL, 
								 &static_deflate, io_static);
			rc2 = pthread_create(&thread_dynamic, NULL, 
								 &dynamic_deflate, io_dynamic);

			if (rc1 || rc2)
				die("thread creation failed");
			pthread_join(thread_static, NULL);
			pthread_join(thread_dynamic, NULL);
			size_static = io_static->result;
			size_dynamic = io_dynamic->result;

			printf("ss = %d, sd = %d, sn = %d\n", size_static, size_dynamic, last_size);
			if (size_dynamic <= size_static && 
				size_dynamic < last_size)
				write_to_output(io_dynamic, output);
			else if (size_static <= size_dynamic && 
					 size_static < last_size)
				write_to_output(io_static, output);
			else
				nocompress_deflate(io_nocom);
		}

		delete_io(&io_static);
		delete_io(&io_dynamic);
		delete_io(&io_nocom);
	}

	free(global_args.output_name);
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

static void do_tests()
{
	test_init_reader();
	test_read_header();
	printf("end of tests\n");
}
