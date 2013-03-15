#include "static_inflate.h"

#include "reader.h"
#include "cyclic_queue.h"


void static_inflate(reader_t *reader)
{
	cyclic_queue *cqbuff = new_cyclic_queue(DICT_SIZE_Q);
	
}
