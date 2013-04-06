#ifndef DYNAMIC_DEFLATE_H
#define DYNAMIC_DEFLATE_H
#include "cyclic_queue.h"

extern cyclic_queue *cqdict;
extern cyclic_queue *cqbuff;

void *dynamic_deflate(void *io_struct);

#endif
