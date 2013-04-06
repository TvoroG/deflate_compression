cmake_minimum_required (VERSION 2.8.4)

set (PROJECT deflate)
project (${PROJECT})

set (deflate_SRCS 
deflate.c
static_deflate.c
dynamic_deflate.c
nocompress_deflate.c
alphabets.c
cyclic_queue.c
huffman_tree.c
writer.c
inflate.c
reader.c
static_inflate.c
dynamic_inflate.c
nocompress_inflate.c
bst.c
tests.c)

find_package (Threads)
add_definitions (-Wall -pedantic)

add_executable (${PROJECT} ${deflate_SRCS})
target_link_libraries (${PROJECT} ${CMAKE_THREAD_LIBS_INIT})