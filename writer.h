#ifndef WRITER_H
#define WRITER_H
#include <stdio.h>
#include "deflate.h"

void write_huffman_code(io *io_s, size_t huff_code, size_t num);
void write_bits(io *io_s, size_t bits, size_t bits_num);
void next_bit(io *io_s);
void byte_flush(io *io_s);

#endif
