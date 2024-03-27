# Custom Memory Management Library

This README provides an overview of the key functions implemented in the custom memory management library, designed as an alternative to the standard `malloc` and `free` functions in C. This library manages a predefined memory buffer for dynamic memory allocation, aiming to optimize memory usage and performance for specific use cases such as embedded systems or applications with constrained memory environments.

## Overview

The library manages memory allocations and deallocations within a fixed-size buffer, incorporating strategies for merging adjacent free memory blocks (coalescing) and recycling freed blocks for new allocations to minimize fragmentation and maximize efficient use of the buffer. It includes debugging functionalities to aid in the development and troubleshooting of memory-related issues.

## Key Data Structures

- `mem_block`: Represents a block of memory within the buffer. It includes metadata such as the size of the block, pointers to the next and previous blocks in the free list, and a magic number for integrity verification.

## Key Functions

### `void *my_malloc(size_t size)`

Allocates a block of memory of at least `size` bytes and returns a pointer to the beginning of the block. If the allocation request cannot be satisfied (e.g., due to insufficient buffer space), `NULL` is returned. This function attempts to recycle free blocks before allocating new space from the buffer.

- **Parameters**:
  - `size`: The number of bytes to allocate.
- **Returns**: A pointer to the allocated memory block, or `NULL` if the allocation fails.

### `void my_free(void *mem)`

Frees a previously allocated block of memory, making it available for future allocations. If `mem` is `NULL`, the function does nothing. It also attempts to coalesce adjacent free blocks into a single larger block to reduce fragmentation.

- **Parameters**:
  - `mem`: A pointer to the memory block to be freed.

### `bool append_block(mem_block *block)`

Appends a memory block to the free list. This function is used internally to manage free memory blocks.

- **Parameters**:
  - `block`: The memory block to append to the free list.
- **Returns**: `true` if the block was successfully appended, `false` otherwise.

### `bool coalesce_block(mem_block *block)`

Attempts to merge a given block with adjacent free blocks in the free list to form a larger continuous block, reducing memory fragmentation.

- **Parameters**:
  - `block`: The memory block to coalesce.
- **Returns**: `true` if the block was successfully merged, `false` otherwise.

### Debugging Functions

- `void print_block(mem_block *block)`: Recursively prints the details of a memory block and its successors in the free list.
- `void print_free_list()`: Prints the entire free list of memory blocks.

## Compilation and Usage

To use this library, include the header file `malloc_v3.h` in your C source files and link against the compiled library. Ensure that the library's source files are compiled with your project or included in your project's source directory.

```c
#include "malloc_v3.h"

int main() {
    // Initialize memory system if necessary
    void *ptr = my_malloc(100);
    if (ptr == NULL) {
        // Handle allocation failure
    }

    // Use allocated memory

    my_free(ptr); // Free when done
    return 0;
}
```

## Notes

This library is designed for educational and experimental purposes and may not be suitable for production environments without further testing and optimization. It offers insights into how low-level memory management and allocation strategies can be implemented in C.
