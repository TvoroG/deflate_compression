#include <string.h>
#include "dynamic_deflate.h"


void dynamic_deflate(off_t block_size, bool isfinal)
{
	two_bytes inter_res[block_size]; /* intermediate result */
	memset(inter_res, 0, block_size);

	byte buff[LEN_MAX];
	size_t count = fread(buff, EL_SIZE, Min(LEN_MAX, block_size), input);
	push_back_cyclic_queue(cqbuff, buff, count);

	byte front_b;
	size_t offset, length, last_count;
	while (!isempty_cyclic_queue(cqbuff)) {
		search_cyclic_queue(cqdict, cqbuff, &offset, &length);
		if (offset == 0 && length == 0) {
			front_b = front_cyclic_queue(cqbuff);
			/* TODO write */
			write_literal();
			move_front_cyclic_queue(cqbuff, cqdict, 1);
		} else {
			/* TODO write*/
			write_pointer();
			move_front_cyclic_queue(cqbuff, cqdict, length);
		}

		if (count < block_size) {
			last_count = fread(buff, EL_SIZE, length, input);
			push_back_cyclic_queue(cqbuff, buff, last_count);
			count += last_count;
		}
	}

}
