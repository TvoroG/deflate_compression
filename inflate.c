#include "inflate.h"

#include <stdio.h>
#include <stdbool.h>

#include "deflate.h"
#include "static_inflate.h"
#include "dynamic_inflate.h"
#include "nocompress_inflate.h"

static bool is_nocompression_type(reader_t *reader);
static bool is_static_type(reader_t *reader);
static bool is_dynamic_type(reader_t *reader);

void inflate(reader_t *reader)
{
	int k = 1;
	do {
		printf("k = %d\n", k++);
		read_block_header(reader);
		if (is_nocompression_type(reader)) {
			printf("nocom\n");
			nocompress_inflate(reader);
		} else if (is_static_type(reader)) {
			printf("static\n");
			static_inflate(reader);
		} else if (is_dynamic_type(reader)) {
			printf("dynamic\n");
			dynamic_inflate(reader);
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
