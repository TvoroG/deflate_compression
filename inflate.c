#include "inflate.h"

#include <stdio.h>
#include <stdbool.h>
#include "deflate.h"

static bool is_nocompression_type(reader_t *reader);
static bool is_static_type(reader_t *reader);
static bool is_dynamic_type(reader_t *reader);



void inflate(reader_t *reader)
{
	do {
		read_block_header(reader);
		if (is_nocompression_type(reader)) {
			printf("invoke nocomp\n");
		} else if (is_static_type(reader)) {
			printf("invoke satic\n");
		} else if (is_dynamic_type(reader)) {
			printf("invoke dynamic\n");
		} else {
			die("error type in inflate\n");
		}
	} while (!reader->isfinal);
}

static bool is_nocompression_type(reader_t *reader)
{
	return reader->compress_type == NO_C ? true : false;
}

static bool is_static_type(reader_t *reader)
{
	return reader->compress_type == STATIC_C ? true : false;
}

static bool is_dynamic_type(reader_t *reader)
{
	return reader->compress_type == DYNAMIC_C ? true : false;
}
