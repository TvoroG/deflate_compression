#include "nocompress_inflate.h"

#include "deflate.h"
#include "writer.h"


void nocompress_inflate(reader_t *reader)
{
	io *io_s;
	init_io(&io_s);

	ignore_byte(reader);
	two_bytes block_size = read_LEN_and_NLEN(reader);

	io_s->block_size = block_size;
	io_s->input = reader->input;
	io_s->output_file = reader->output;

	unget_last_byte(reader);
	write_data(io_s);

	io_s->input = NULL;
	io_s->output_file = NULL;
	delete_io(&io_s);
}
