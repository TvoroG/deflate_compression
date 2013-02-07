#ifndef STATIC_DEFLATE_H
#define STATIC_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "deflate.h"
#include "cyclic_queue.h"

extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void *static_deflate(void *io_s);

static void write_literal(io *io_s, two_bytes code);
static void write_pointer(io *io_s, size_t length, size_t offset);
static void write_static_header(io *io_s);
static void write_end_of_block(io *io_s);
static void prepare_input_file(io *io_s);
static size_t get_output_size(io *io_s);
#endif

