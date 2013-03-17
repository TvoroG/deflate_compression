#ifndef DYNAMIC_INFLATE_H
#define DYNAMIC_INFLATE_H

#include "reader.h"

void dynamic_inflate(reader_t *reader);
void build_huffman_codes(huffman_code huff_code[], size_t size);
void print_huffman_codes(huffman_code huff_code[], size_t size);

#endif
