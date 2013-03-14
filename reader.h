#ifndef READER_H
#define READER_H
#include <stdio.h>
#include <stdbool.h>
#include "deflate.h"

typedef enum {NO_C, STATIC_C, DYNAMIC_C, ERROR_C} compress_t;

typedef struct _reader_t {
	char *input_name;
	FILE *input;
	byte read_b;
	byte read_i;
	bool isfinal;
	compress_t compress_type;
} reader_t;

void init_reader(reader_t **reader);
void delete_reader(reader_t **reader);
void read_block_header(reader_t *reader);
void read_next_bit(reader_t *reader);
void read_next_byte(reader_t *reader);

#endif
