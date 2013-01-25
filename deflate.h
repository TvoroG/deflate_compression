#ifndef DEFLATE_H
#define DEFLATE_H
#include <stdio.h>
#include <stdlib.h>

#define INPUT "input"
#define OUTPUT "output"
#define N 8
#define GetBit(reg, bit)   (reg & (1 << bit) ? 1 : 0)
#define SetBit(reg, bit)   (reg |= (1<<bit))
#define ClearBit(reg, bit) (reg &= (~(1<<bit)))
#define DICT_SIZE_Q 32769 /*32768 + 1*/
#define LEN_SIZE_Q 259 /*258 + 1*/

typedef unsigned char byte;
void die(char *);
void print_bytes(int, size_t);
void static_deflate(FILE *, FILE *);

#endif
