#include "reader.h"

void init_reader(reader_t **reader)
{
	*reader = (reader_t *) malloc(sizeof(reader_t));
	
	(*reader)->input_name = global_args.input_name;
	(*reader)->input = fopen((*reader)->input_name, "r");

	read_next_byte(*reader);
	(*reader)->read_i = 0;

	(*reader)->isfinal = false;
	(*reader)->compress_type = ERROR_C;
}

void delete_reader(reader_t **reader)
{
	if ((*reader)->input != NULL)
		fclose((*reader)->input);

	free(*reader);
	*reader = NULL;
}

void read_block_header(reader_t *reader)
{
	if (BitIsSet(reader->read_b, reader->read_i))
		reader->isfinal = true;
	read_next_bit(reader);

	byte b = 0;
	if (BitIsSet(reader->read_b, reader->read_i))
		SetBit(b, 0);
	read_next_bit(reader);
	if (BitIsSet(reader->read_b, reader->read_i))
		SetBit(b, 1);
	read_next_bit(reader);

	if (b == 0)
		reader->compress_type = NO_C;
	else if (b == 1)
		reader->compress_type = STATIC_C;
	else if (b == 2)
		reader->compress_type = DYNAMIC_C;
	else
		reader->compress_type = ERROR_C;
}

void read_next_bit(reader_t *reader)
{
	reader->read_i++;
	if (reader->read_i >= N) {
		reader->read_i = 0;
		read_next_byte(reader);
	}
}

void read_next_byte(reader_t *reader)
{
	byte b;
	fread(&b, sizeof(byte), 1, reader->input);
	reader->read_b = b;
}
