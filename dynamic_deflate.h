#ifndef DYNAMIC_DEFLATE_H
#define DYNAMIC_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include "deflate.h"
#include "cyclic_queue.h"

#define FIRST_LEN_EXTRA_BIT 9
#define LAST_LEN_EXTRA_BIT 13
#define LAST1_OFF_BIT 15
#define FIRST2_OFF_BIT 0
#define LAST2_OFF_BIT 2
#define LAST_OFF_EXTRA_BIT 15

extern FILE *input;
extern FILE *output;
extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void dynamic_deflate(off_t, bool);
off_t LZ77(two_bytes [], off_t);
size_t write_pointer(two_bytes [],
					 size_t, 
					 size_t, 
					 size_t);
void write_len_extra_bits(two_bytes [], size_t, two_bytes);
void write_offset(two_bytes [], size_t, byte);
void write_off_extra_bits(two_bytes [], size_t, two_bytes);
#endif
