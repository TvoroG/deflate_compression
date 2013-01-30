#ifndef DEFLATE_H
#define DEFLATE_H
#include <stdio.h>
#include <stdlib.h>

#define GetBit(reg, bit)   ((reg) & (1 << (bit)) ? 1 : 0)
#define SetBit(reg, bit)   ((reg) |= (1<<(bit)))
#define ClearBit(reg, bit) ((reg) &= (~(1<<(bit))))
#define BitIsSet(reg, bit) (((reg) & (1 << (bit))) != 0)
#define Min(a, b) ((a) >= (b) ? (b) : (a))

#define N 8
#define INPUT "input"
#define OUTPUT "output"

#define HEADER_LEN 3
#define DICT_SIZE_Q 32769 /*32768 + 1*/
#define LEN_SIZE_Q 259 /*258 + 1*/
#define DICT_MAX 32768
#define LEN_MAX 258
#define EL_SIZE 1 /* element size */
#define MAX_LITLEN_CODE 285
#define MAX_OFF_CODE 29

typedef unsigned char byte;
typedef unsigned short two_bytes;

void die(char *);
void print_bytes(int, size_t);

#endif
