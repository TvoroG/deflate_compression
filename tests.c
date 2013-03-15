#include "tests.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "deflate.h"
#include "reader.h"

/* reader */
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

void test_is_in_huffman_code()
{
	reader_t *reader;
	init_reader(&reader);
	read_block_header(reader);
	assert(is_in_huffman_code(51, 0) == true);
	assert(is_in_huffman_code(403, 1) == true);
	assert(is_in_huffman_code(5, 2) == true);
	assert(is_in_huffman_code(195, 3) == true);
	assert(is_in_huffman_code(52, 3) == false);
	assert(is_in_huffman_code(405, 0) == false);
	assert(is_in_huffman_code(191, 0) == true);
	assert(is_in_huffman_code(511, 1) == true);
	assert(is_in_huffman_code(23, 2) == true);
	assert(is_in_huffman_code(199, 3) == true);
	printf("OK\n");
}

void test_decode_next_litlen()
{
	reader_t *reader;
	init_reader(&reader);
	read_block_header(reader);
	assert(decode_next_litlen(reader) == 'D');
	assert(decode_next_litlen(reader) == 'e');
	assert(decode_next_litlen(reader) == 'f');
	assert(decode_next_litlen(reader) == 'l');
	assert(decode_next_litlen(reader) == 'a');
	assert(decode_next_litlen(reader) == 't');
	assert(decode_next_litlen(reader) == 'e');
	assert(decode_next_litlen(reader) == ' ');
	assert(decode_next_litlen(reader) != 'p');
	delete_reader(&reader);
	printf("OK\n");
}

/* inflate */

/* static_inflate */
