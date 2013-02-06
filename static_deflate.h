#ifndef STATIC_DEFLATE_H
#define STATIC_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "deflate.h"
#include "cyclic_queue.h"

extern FILE *input;
extern FILE *output;
extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void static_deflate(bool);

static void write_literal(two_bytes);
static void write_pointer(size_t, size_t);
static void write_static_header(bool);
#endif
