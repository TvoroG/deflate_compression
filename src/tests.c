#include "tests.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "deflate.h"
#include "reader.h"
#include "cyclic_queue.h"
#include "bst.h"

/* reader */
void test_init_reader()
{
	reader_t *reader;
	init_reader(&reader);
	assert(reader->output != NULL);
	assert(reader->isfinal == false);
	assert(reader->compress_type == ERROR_C);
	assert(reader->input != NULL);
	assert(reader->read_b != 0);
	assert(reader->read_i == 0);
	delete_reader(&reader);
	assert(reader == NULL);
	printf("OK\n");
}

void test_read_header()
{
	reader_t *reader;
	init_reader(&reader);
	read_block_header(reader);
	assert(reader->compress_type != ERROR_C);
	assert(reader->read_i == 3);
	delete_reader(&reader);
	printf("OK\n");
}

void test_is_in_huffman_code()
{
	reader_t *reader;
	init_reader(&reader);
	assert(is_in_huffman_code(51, 0) == true);
	assert(is_in_huffman_code(403, 1) == true);
	assert(is_in_huffman_code(5, 2) == true);
	assert(is_in_huffman_code(195, 3) == true);
	assert(is_in_huffman_code(52, 3) == false);
	assert(is_in_huffman_code(405, 0) == false);
	assert(is_in_huffman_code(191, 0) == true);
	assert(is_in_huffman_code(511, 1) == true);
	assert(is_in_huffman_code(23, 2) == true);
	assert(is_in_huffman_code(199, 3) == true);
	printf("OK\n");
}

void test_decode_next_litlen()
{
	reader_t *reader;
	init_reader(&reader);
	read_block_header(reader);
	delete_reader(&reader);
	printf("OK\n");
}

/* cyclic_queue */

void test_get_cyclic_queue()
{
	int s = 7;
	cyclic_queue *cq = new_cyclic_queue(s, false);
	add_cyclic_queue(cq, 'D');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'f');
	add_cyclic_queue(cq, 'l');
	add_cyclic_queue(cq, 'a');
	add_cyclic_queue(cq, 't');
	add_cyclic_queue(cq, 'e');
	print_cyclic_queue(cq);
	byte bytes[s];
	int len = 1;
	int off = 1;
	get_cyclic_queue(cq, bytes, len, off);
	int i = 0;
	for (i = 0; i < len; i++) {
		printf("%c, ", bytes[i]);
	}
	printf("\n");
}

void test_new_search_cyclic_queue()
{
	int s = 7;
	cyclic_queue *cq = new_cyclic_queue(s, true);
	byte *b = malloc(10);
	strcpy(b, "deflateeg");
	push_back_cyclic_queue(cq, b, 9);
	assert(cq->bst->index_s == 3);
	delete_cyclic_queue(cq);
	printf("OK\n");
}

void test_index_to_ptr()
{
	int s = 7;
	cyclic_queue *cq = new_cyclic_queue(s, false);
	add_cyclic_queue(cq, 'D');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'f');
	add_cyclic_queue(cq, 'l');
	add_cyclic_queue(cq, 'a');
	add_cyclic_queue(cq, 't');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'e');
	print_cyclic_queue(cq);
	assert(index_to_ptr(cq, 0) == 4);
	assert(index_to_ptr(cq, 1) == 5);
	assert(index_to_ptr(cq, 5) == 2);
	printf("OK\n");
}

void test_ptr_to_index()
{
	int s = 7;
	cyclic_queue *cq = new_cyclic_queue(s, false);
	add_cyclic_queue(cq, 'D');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'f');
	add_cyclic_queue(cq, 'l');
	add_cyclic_queue(cq, 'a');
	add_cyclic_queue(cq, 't');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'e');
	add_cyclic_queue(cq, 'e');
	print_cyclic_queue(cq);
	assert(ptr_to_index(cq, index_to_ptr(cq, 0)) == 0);
	assert(ptr_to_index(cq, index_to_ptr(cq, 1)) == 1);
	assert(ptr_to_index(cq, index_to_ptr(cq, 3)) == 3);
	assert(ptr_to_index(cq, index_to_ptr(cq, 5)) == 5);

	assert(index_to_ptr(cq, ptr_to_index(cq, 0)) == 0);
	assert(index_to_ptr(cq, ptr_to_index(cq, 1)) == 1);
	assert(index_to_ptr(cq, ptr_to_index(cq, 4)) == 4);
	assert(index_to_ptr(cq, ptr_to_index(cq, 5)) == 5);
	printf("OK\n");
}

/* inflate */

/* static_inflate */

/* dynamic_inflate */
void test_huffman_codes()
{
	size_t size = 8;
	huffman_code huff_code[] = {
		{3, 0},
		{3, 0},
		{3, 0},
		{3, 0},
		{3, 0},
		{2, 0},
		{4, 0},
		{4, 0}
	};

	build_huffman_codes(huff_code, size);
	assert(huff_code[0].code == 2);
	assert(huff_code[1].code == 3);
	assert(huff_code[5].code == 0);
	assert(huff_code[7].code == 15);
	printf("OK\n");
}

/* bst */
void test_new_bst()
{
	bst_t *bst = new_bst();
	assert(bst == NULL);
	printf("OK\n");
}

void test_get_substr_len_bst()
{
	assert(get_substr_len_bst("wowop",5, "wowgo",5) == 3);
	assert(get_substr_len_bst("gohome", 6,"gohomeolol",10) == 6);
	assert(get_substr_len_bst("bestgame",8, "best",4) == 4);
	assert(get_substr_len_bst("deflate",7, "deflate",7) == 7);
	assert(get_substr_len_bst("", 0,"omg",3) == 0);
	printf("OK\n");
}

/*
void test_insert_bst()
{
	bst_t *bst = new_bst();
	insert_bst(&bst, "wow", 10);
	insert_bst(&bst, "aaa", 10);
	insert_bst(&bst, "bbb", 5);
	insert_bst(&bst, "yyy", 10);
	insert_bst(&bst, "000", 99);
	insert_bst(&bst, "AAA", 99);
	insert_bst(&bst, "###", 99);
	insert_bst(&bst, "", 0);
	insert_bst(&bst, "xxx", 99);
	insert_bst(&bst, "zzz", 99);
	clean_bst(&bst, 1000, 6);
//	clean_bst(&bst, 55, 100);
	print_bst(bst);
	printf("OK\n");
}
*/

/*
void test_delete_bst()
{
	bst_t *bst = new_bst();
	insert_bst(&bst, "wow", 10);
	insert_bst(&bst, "aaa", 10);
	insert_bst(&bst, "bbb", 5);
	insert_bst(&bst, "yyy", 10);
	insert_bst(&bst, "000", 99);
	delete_bst(&bst);
	print_bst(bst);
	assert(bst == NULL);
	printf("OK\n");
}
*/
/*
void test_search_bst()
{
	bst_t *bst = new_bst();
	insert_bst(&bst, "wow", 10);
	insert_bst(&bst, "aaa", 10);
	insert_bst(&bst, "bbb", 5);
	insert_bst(&bst, "linux&windows", 45);
	insert_bst(&bst, "yyy", 10);
	insert_bst(&bst, "000", 99);	
	size_t len;
	bst_t *res = search_bst(bst, "bb", 2, &len);
	assert(len == 2);
	assert(res->ptr == 5);
	res = search_bst(bst, "linux", 5, &len);
	assert(len == 5);
	assert(res->ptr == 45);
	printf("OK\n");
}
*/
