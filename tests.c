#include "tests.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "deflate.h"
#include "reader.h"

void test_init_reader()
{
	reader_t *reader;
	init_reader(&reader);
	assert(reader->isfinal == false);
	assert(reader->compress_type == ERROR_C);
	assert(reader->input != NULL);
	assert(reader->read_b != 0);
	assert(reader->read_i == 0);
	delete_reader(&reader);
	assert(reader == NULL);
	printf("OK\n");
}

void test_read_header()
{
	reader_t *reader;
	init_reader(&reader);
	read_block_header(reader);
	assert(reader->isfinal == true); /* ? */
	assert(reader->compress_type != ERROR_C);
	assert(reader->read_i == 3);
	delete_reader(&reader);
	printf("OK\n");
}
