#ifndef STATIC_DEFLATE_H
#define STATIC_DEFLATE_H
#include "cyclic_queue.h"

extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void *static_deflate(void *io_s);

#endif
