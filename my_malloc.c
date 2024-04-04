/* Version 3 Of Malloc Returns Freeable Blocks */
#include "my_malloc.h"
#include <stdio.h>


/* malloc's memory buffer */
void *buffer = NULL;
/* the amount memory used from the buffer */
size_t buffer_used = 0;
/* the total size of the buffer */
size_t buffer_size = 0;
/* list of free memory blocks */
mem_block *free_list = NULL;


mem_block *get_block(void *ptr)
{
    mem_block *block;
    if (isnull(ptr))
        return NULL;
    
    block = (mem_block *)((char *)ptr - META_SIZE);
    return block;
}


/**
 * check if two memory blocks are neighbours
 * @param block: the memory block
 * @param neighbour: the neighbour
 * @return the side of the neighbour
*/
n_side is_neighbour(mem_block *block, mem_block *neighbour)
{
    mem_block *temp;

    if (isnull(block) || isnull(neighbour)) {
        return none;
    }
    /**
     * check if the address after this memory block + the usable chunk
     * is the neighbour's address
     **/
    temp = (mem_block *)((char *)block + META_SIZE + block->size);
    if (temp == neighbour) {
        return right;
    }
    /**
     * check if the address after the neighbour's memory block + the usable chunk
     * is the block's address
     **/
    temp = (mem_block *)((char *)neighbour + META_SIZE + neighbour->size);
    if (temp == block) {
        return left;
    }
    return none;
}


/**
 * append a block to the free list
 * @param block: the block to append
 * @return a boolean indicating if the block was appended
*/
bool append_block(mem_block *block)
{
    mem_block *tail;
    debug("[*] appending block to free list: ");
    print_block(block);

    if (isnull(block)) {
        return false;
    }
    if (isnull(free_list)) {
        free_list = block;
        return true;
    }
    tail = free_list;
    while (!isnull(tail->next)) {
        tail = tail->next;
    }
    tail->next = block;
    block->prev = tail;
    return true;
}


/**
 * merge a recently freed block into a bloc on the freelist if possible
 * @param block: the block to merge
 * @return a boolean indicating if the block was merged
*/
bool coalesce_block(mem_block *block)
{
    mem_block *new_block;
    size_t new_size;
    n_side side;
    mem_block *neighbour = free_list;

    debug("[*] coalescing block ");
    print_block(block);

    if (isnull(block) || isnull(free_list)) {
        debug("Block or free list is null\n");
        return false;
    }

    while (!isnull(neighbour)) {
        side = is_neighbour(block, neighbour);
        if (side != none)
            break;
        neighbour = neighbour->next;
    }
    if (isnull(neighbour)) {
        debug("No neighbour found\n");
        return false;
    }
    new_size = block->size + neighbour->size + META_SIZE;
    if (side == right) {
        debug("Found neighbour on the right: ");
        print_block(neighbour);
        new_block = block;
        new_block->size = new_size;
        new_block->next = neighbour->next;
        new_block->prev = neighbour->prev;
        if (!isnull(neighbour->prev))
            neighbour->prev->next = new_block;
        if (!isnull(neighbour->next))
            neighbour->next->prev = new_block;
        /* update the free list */
        if (neighbour == free_list)
            free_list = new_block;
        /* clear the metadata from the merged block */
        initialize_mem((char *)neighbour, META_SIZE);
    } else {
        debug("Found neighbour on the left: ");
        print_block(neighbour);
        new_block = neighbour;
        new_block->size = new_size;
        new_block->next = block->next;
        new_block->prev = block->prev;
        if (!isnull(block->prev))
            block->prev->next = new_block;
        if (!isnull(block->next))
            block->next->prev = new_block;

        /* clear the metadata from the merged block */
        initialize_mem((char *)block, META_SIZE);
    }
    return true;
}

/**
 * initialize memory block to zero
 * @param mem: the memory block
 * @param size: the size of the memory block
 * @return the initialized memory block
 */
char *initialize_mem(char *mem, size_t size)
{
    uint i;

    for (i = 0; i < size; i++)
        mem[i] = 0;
    return mem;
}

/**
 * allocate new memory
 * @param size: the size of the memory to allocate
 * @return the newly allocated memory block
*/
mem_block *allocate_memory(size_t size)
{
    /* the current program break */
    void *prog_brk;
    /* the new memory created */
    char *new_mem;
    mem_block *block;

    if (buffer == NULL)
        return NULL;
    if (size == 0)
        return NULL;

    debug("[*]allocating %lu bytes...\n", size);

    /**
     * while the current current space left in the
     * buffer is less than what the user needs, keep increasing
     * the buffer
     */
    while ((buffer_size - buffer_used) < size + META_SIZE)
    {
        debug("Increasing buffer size by %lu\n", MALLOC_BUF_INCR);
        prog_brk = sbrk(MALLOC_BUF_INCR);
        if (prog_brk == (void *)-1)
            return NULL;
        debug("New program break: %p\n", prog_brk);
        buffer_size += MALLOC_BUF_INCR;
        debug("Current buffer size: %lu\n", buffer_size);
    }

    new_mem = (char *)buffer + buffer_used;
    buffer_used += META_SIZE + size;
    initialize_mem(new_mem + META_SIZE, size);
    block = (mem_block *)new_mem;
    block->size = size;
    block->next = NULL;
    block->prev = NULL;
    block->flag = BLOCK_ALLOCATED;
    debug("Created new memory block: ");
    print_block(block);
    return (block);
}


/**
 * recycle memory
 * @param size: the size of the memory to recycle
 * @return the recycled memory block
*/
mem_block *recycle_memory(size_t size)
{
    mem_block *block, *tail;

    debug("[*] recycling %lu bytes...\n", size);

    if (isnull(free_list) || size == 0)
        return NULL;

    block = free_list;
    tail = NULL;

    /* find best-fit */
    while (!isnull(block))
    {
        if (block->size == size)
        {
            debug("Best fit found: ");
            print_block(block);
            if (!isnull(block->next))
                block->next->prev = block->prev;
            if (!isnull(block->prev))
                block->prev->next = block->next;
            block->next = NULL;
            block->prev = NULL;
            block->flag = BLOCK_ALLOCATED;
            if (free_list == block)
            { /* is head of list */
                free_list = block->next;
            }
            return block;
        }
        tail = block;
        block = block->next;
    }

    while (!isnull(tail))
    {
        /* check if the size is enough to contain a new memory block */
        if (tail->size > size + META_SIZE)
        {
            block = tail;
            debug("First fit found: ");
            print_block(block);
            /* split the memory */
            tail = (mem_block *)((char *)block + META_SIZE + size);
            tail->size = block->size - size - META_SIZE;
            tail->next = block->next;
            tail->prev = block->prev;
            tail->flag = BLOCK_FREED;
            if (!isnull(tail->next))
                tail->next->prev = tail;
            if (!isnull(tail->prev))
                tail->prev->next = tail;
            else /* is head of list */
                free_list = tail;
            block->size = size;
            block->next = NULL;
            block->prev = NULL;
            block->flag = BLOCK_ALLOCATED;
            return block;
        }
        tail = tail->prev;
    }
    return NULL;
}


/**
 * recursively print a memory block object
 * @param block: pointer to memory block
 * @return nothing!
 */
void print_block(mem_block *block)
{
    char *symbol;
    if (!DEBUG)
        return;
    if (isnull(block))
    {
        printf("<empty>\n");
        return;
    }
    
    if (block->flag == BLOCK_ALLOCATED)
        symbol = "A"; /* Symbol for allocated block */
    else if (block->flag == BLOCK_FREED)
        symbol = "U"; /* Symbol for freed block */
    else
        symbol = "U"; /* Symbol for unknown block */
    /* Print left arrow if there is a previous block */
    if (!isnull(block->prev))
        printf("← ");

    printf("%s[%p(%lu)] ", symbol, (char *)block + META_SIZE, block->size);

    /* Print right arrow if there is a next block */
    if (block->next)
        printf("→ ");


    /* Recursively print the next block */
    if (block->next)
        print_block(block->next);

    if (isnull(block->prev))
        printf("\n");
}

/**
 * print the free list
 */
void print_free_list()
{
    debug("Free list: ");
    print_block(free_list);
}

/**
 * allocated `size` bytes of memory
 * @param size: the size of memory to allocated
 * @return A pointer to the start of the allocated block
 */
void *malloc(size_t size)
{
    void *block;
    /* initialize the buffer */
    if (isnull(buffer))
    {
        debug("Initial program break %p\n", sbrk(0));
        buffer = (void *)sbrk(1);  /* primes sbrk */
        debug("Program break after priming %p\n", buffer);
        debug("Initialized buffer\n", buffer);
    }

    debug("\nMalloc request for %lu bytes\n", size);
    
    /* allocate the memory */
    debug("Free list before recycling: ");
    print_block(free_list);
    block = recycle_memory(size);
    if (isnull(block))
    {
        debug("Failed to recycle memory\n");
        block = allocate_memory(size);
        if (isnull(block))
        {
            debug("Failed to allocate memory\n");
            return NULL;
        }
    }
    else
    {
        debug("Free list after recycling: ");
        print_block(free_list);
    }
    debug("Allocated block: ");
    print_block(block);
    return ((char *)block + META_SIZE);
}

/**
 * free a block of memory allocated by my_malloc
 * @param ptr: a pointer to a block of memory
*/
void free(void *ptr)
{
    mem_block *block;

    if (isnull(ptr)) return;
    block = get_block(ptr);

    debug("\nFreeing block: ");
    print_block(block);

    if (block->flag == BLOCK_FREED) {
        error("Block has already been freed\n");
        return;
    }
    else if (block->flag != BLOCK_ALLOCATED) {
        error("Attempt to free a block that was not allocated by my_malloc\n");
        return;
    }
    block->flag = BLOCK_FREED;
    if (coalesce_block(block) == false) {
        debug("Failed to coalesce block\n");
        if (append_block(block) == false) {
            error("Failed to free block\n");
            return;
        }
    }
}


/**
 * allocate memory for an array of nmemb elements of size bytes each
 * @param nmemb: the number of elements
 * @param size: the size of each element
 * @return a pointer to the allocated memory
*/
void *calloc(size_t nmemb, size_t size)
{
    void *block;
    size_t total_size;

    total_size = nmemb * size;
    block = malloc(total_size);
    if (isnull(block))
        return NULL;
    initialize_mem(block, total_size);
    return block;
}

/**
 * change the size of the memory block pointed to by ptr to size bytes.
 * @param ptr: a pointer to the memory block
 * @param size: the new size of the memory block
 * @return a pointer to the new memory block
*/
void *realloc(void *ptr, size_t size)
{
    mem_block *block;
    void *new_block;
    size_t old_size;

    if (isnull(ptr))
        return malloc(size);
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }
    block = get_block(ptr);
    old_size = block->size;
    if (old_size == size)
        return ptr;
    new_block = malloc(size);
    if (isnull(new_block))
        return NULL;
    if (old_size < size)
        size = old_size;
    memcpy(new_block, ptr, size);
    free(ptr);
    return new_block;
}


/**
 * change the size of the memory block pointed to by ptr to nmemb * size bytes.
 * @param ptr: a pointer to the memory block
 * @param nmemb: the number of elements
 * @param size: the size of each element
 * @return a pointer to the new memory block
*/
void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
    return realloc(ptr, nmemb * size);
}


/**
 * perform cleanup
*/
void cleanup()
{
    if (buffer != NULL)
    {
        debug("\nCleaning up...\n");
        brk(buffer); /* Free the memory buffer */
        debug("Program break after cleanup %p\n\n", sbrk(0));
        buffer = NULL;
        buffer_size = 0;
        buffer_used = 0;
        free_list = NULL;
    }
}