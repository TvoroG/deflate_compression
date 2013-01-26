#ifndef DEFLATE_H
#define DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INPUT "input"
#define OUTPUT "output"
#define N 8
#define GetBit(reg, bit)   ((reg) & (1 << (bit)) ? 1 : 0)
#define SetBit(reg, bit)   ((reg) |= (1<<(bit)))
#define ClearBit(reg, bit) ((reg) &= (~(1<<(bit))))
#define BitIsSet(reg, bit) (((reg) & (1 << (bit))) != 0)
#define DICT_SIZE_Q 32769 /*32768 + 1*/
#define LEN_SIZE_Q 259 /*258 + 1*/
#define DICT_MAX 32768
#define LEN_MAX 258
#define EL_SIZE 1 /* element size */

typedef unsigned char byte;
void die(char *);
void print_bytes(int, size_t);
void static_deflate(FILE *, FILE *, bool);
void write_literal(FILE *, byte);
void write_pointer(FILE *, size_t, size_t);
void write_static_header(FILE *, bool);
void next_bit(FILE *);
void byte_flush(FILE *);

#endif
