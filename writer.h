#ifndef WRITER_H
#define WRITER_H
#include <stdio.h>

void write_huffman_code(size_t huff_code, size_t num);
void write_bits(size_t bits, size_t bits_num);
void next_bit();
void byte_flush();

#endif
