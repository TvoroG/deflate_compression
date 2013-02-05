#ifndef NOCOMPRESS_DEFLATE_H
#define NOCOMPRESS_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "deflate.h"

#define BUFF_SIZE 100

void nocompress_deflate(off_t block_size, bool isfinal);
static void write_nocompress_header(bool isfinal);
static void write_length_of_block(off_t block_size);
static void write_data(off_t block_size);
static void write_bits(two_bytes bits, size_t bits_num);
static void next_bit();
static void byte_flush();

#endif
