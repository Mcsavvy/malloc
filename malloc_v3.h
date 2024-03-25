#ifndef MALLOC_V3_H
#define MALLOC_V3_H

#include <unistd.h>
#include "debug.h"


#define MALLOC_BUF_INCR 0x21000
#define BLOCK_FREED 0xFEE
#define BLOCK_ALLOCATED 0xF00
#define META_SIZE sizeof(mem_block)
#define isnull(ptr) (ptr == NULL)

/**
 * a boolean type
 * @param true: true
 * @param false: false
*/
typedef enum Bool {
    false,
    true
} bool;

/* unsigned integer */
typedef unsigned int uint;

/**
 * start of memory block
 * 
 * @param size: the size of the memory block
 * @param next: the next memory block
 * @param flag: a flag indicating if the memory block is free or allocated
 * */
typedef struct mem_block
{
    size_t size;
    struct mem_block *prev;
    struct mem_block *next;
    uint flag;
} mem_block;

/**
 * neighbouring side of a memory block
 * @param left: the left side of a memory block
 * @param right: the right side of a memory block
 * @param none: no side
*/
typedef enum n_side {
    left,
    right,
    none
} n_side;

n_side is_neighbour(mem_block *block, mem_block *neighbour);
bool append_block(mem_block *block);
bool coalesce_block(mem_block *block);
void debug(const char *format, ...);
char *initialize_mem(char *mem, size_t size);
void print_block(mem_block *block);
mem_block *get_block(void *mem);
void print_free_list(void);
mem_block *allocate_memory(size_t size);
mem_block *recycle_memory(size_t size);
void *my_malloc(size_t size);
void my_free(void *mem);
void cleanup(void);


#endif