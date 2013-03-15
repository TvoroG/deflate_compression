#include "static_inflate.h"

#include "reader.h"
#include "cyclic_queue.h"
#include "alphabets.h"
#include "writer.h"


void static_inflate(reader_t *reader)
{
	cyclic_queue *cqbuff = new_cyclic_queue(DICT_SIZE_Q);
	io *io_s;
	init_io(&io_s);

	two_bytes litlen;
	bool is_end_of_block = false;
	while (!is_end_of_block) {
		litlen = decode_next_litlen(reader);
		if (litlen < 256)
			write_bits(io_s, litlen, N);
		else if (litlen == END_OF_BLOCK)
			is_end_of_block = true;
		else {
			two_bytes length = decode_length(reader, litlen);
			two_bytes distance = decode_distance(reader);
			printf("length = %d, distance = %d\n", length, distance);
		}
	}
	
	write_to_output(io_s, reader->output);
}
