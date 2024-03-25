#include "malloc_v3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test function prototypes */
void test_allocating_no_memory();
void test_allocating_large_memory();
void test_allocating_memory();
void test_freeing_memory();
void test_recycling_memory();

int main()
{
    test_allocating_no_memory();
    cleanup();
    test_allocating_memory();
    cleanup();
    test_allocating_large_memory();
    cleanup();
    test_freeing_memory();
    cleanup();
    test_recycling_memory();

    return 0;
}

void test_allocating_no_memory()
{
    void *ptr = my_malloc(0);
    printf("Testing allocation of 0 bytes\n");
    assert(ptr == NULL); /* Expect NULL since no memory should be allocated */
    printf("Allocation of 0 bytes test passed\n");
}


void test_allocating_memory()
{
    char *text = "My Memory Block!";
    size_t size;
    char *ptr;

    size = strlen(text) + 1; /* Include the null terminator */
    printf("-> Testing allocation of memory\n");
    ptr = (char *)my_malloc(size);
    ptr[size] = 0;
    assert(ptr != NULL); /* Expect a valid pointer */
    assert(get_block(ptr)->size == size); /* Ensure the block is the correct size */
    assert(get_block(ptr)->flag == BLOCK_ALLOCATED); /* Ensure the block is allocated */
    strcpy(ptr, text); /* Write to the memory block */
    assert(strcmp(ptr, text) == 0); /* Ensure the data was written correctly */
    my_free(ptr);        /* Clean up */
    printf("Allocation of memory test passed\n");
}

void test_allocating_large_memory()
{
    size_t large_size = MALLOC_BUF_INCR * 3;
    void *ptr;
    printf("Testing allocation of large memory block (%lu bytes)\n", large_size);
    ptr = my_malloc(large_size);
    assert(ptr != NULL); /* Expect a valid pointer */
    assert(get_block(ptr)->size == large_size); /* Ensure the block is the correct size */
    assert(get_block(ptr)->flag == BLOCK_ALLOCATED); /* Ensure the block is allocated */
    my_free(ptr);        /* Clean up */
    printf("Allocation of large memory test passed\n");
}

void test_freeing_memory()
{
    size_t size;
    void *ptr;
    printf("Testing freeing memory\n");
    size = 128; /* A moderate size for testing */
    ptr = my_malloc(size);
    assert(ptr != NULL); /* Ensure allocation was successful */
    my_free(ptr);
    assert(get_block(ptr)->flag == BLOCK_FREED); /* Ensure the block is freed */
    printf("Freeing memory test passed\n");
}

void test_recycling_memory()
{
    size_t size;
    void *ptr1, *ptr2, *ptr3;

    printf("Testing recycling memory\n");
    size = META_SIZE * 3;
    ptr1 = my_malloc(size);
    assert(ptr1 != NULL);
    assert(get_block(ptr1)->size == size); /* Ensure the block is the correct size */
    assert(get_block(ptr1)->flag == BLOCK_ALLOCATED); /* Ensure the block is marked as allocated */
    my_free(ptr1); /* Free the block to allow for recycling */

    /* Attempt to allocate a same-sized block to test best fit */
    ptr2 = my_malloc(size);
    assert(ptr2 == ptr1); /* Expect the same block to be recycled */
    assert(get_block(ptr2)->size == size); /* Ensure the block is the correct size */
    assert(get_block(ptr2)->flag == BLOCK_ALLOCATED); /* Ensure the block is marked as allocated */
    my_free(ptr2);

    /* Attempt to allocate a smaller block to test first fit */
    ptr3 = my_malloc(META_SIZE);
    assert(get_block(ptr3) == get_block(ptr1)); /* Expect the same block to be recycled */
    assert(get_block(ptr3)->size == META_SIZE); /* Ensure the block is the correct size */
    assert(get_block(ptr3)->flag == BLOCK_ALLOCATED); /* Ensure the block is marked as allocated */
    my_free(ptr3);

    printf("Recycling memory test passed\n");
}
