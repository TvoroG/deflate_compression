#include "static_inflate.h"

#include "reader.h"
#include "cyclic_queue.h"
#include "alphabets.h"
#include "writer.h"


void static_inflate(reader_t *reader)
{
	io *io_s;
	init_io(&io_s);
	byte bytes[LEN_MAX];

	two_bytes litlen;
	bool is_end_of_block = false;
	while (!is_end_of_block) {
		litlen = decode_next_litlen(reader);
		if (litlen < END_OF_BLOCK)
			write_byte(io_s, litlen);
		else if (litlen == END_OF_BLOCK)
			is_end_of_block = true;
		else {
			two_bytes length = decode_length(reader, litlen);
			two_bytes offset_code = read_bits(reader, OFF_CODE_LEN);
			two_bytes distance = decode_distance(reader, offset_code);
			get_cyclic_queue(io_s->output, bytes, length, distance);
			write_bytes(io_s, bytes, length);
		}
	}
	
	write_to_output(io_s, reader->output);
	delete_io(&io_s);
}
