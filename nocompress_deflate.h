#ifndef NOCOMPRESS_DEFLATE_H
#define NOCOMPRESS_DEFLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "deflate.h"

#define BUFF_SIZE 100

void nocompress_deflate(io *io_s);

#endif
