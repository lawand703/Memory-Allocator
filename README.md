# Memory Allocator

This project implements a custom memory allocator in C, providing functionalities for memory allocation (`mymalloc()`) and deallocation (`myfree()`). The allocator supports three different strategies: BEST_FIT, FIRST_FIT, and WORST_FIT, which determine how the allocator selects empty blocks from the memory pool.

## Strategies

### BEST_FIT
- **Test 1**: Freeing and allocating a small block (size of an integer).
  - This strategy selects the smallest empty block that fits the requested size, minimizing wasted memory and providing optimal allocation.
  - Typically yields better results compared to other strategies in terms of external fragmentation.

### FIRST_FIT
- **Test 2**: Allocating a large block (size of 1024 integers).
  - This strategy selects the first available block that meets the requested size for allocation.
  - Often not preferred due to potentially inefficient memory usage, leading to increased external fragmentation.

### WORST_FIT
- **Test 3**: Freeing a large block.
  - This strategy selects the largest available block, even if it's larger than the requested size, for deallocation.
  - Similar to FIRST_FIT, it's not commonly favored due to its tendency to increase external fragmentation.

## Getting Started

To run the project, follow these steps:

1. Compile the source code.
2. Run the compiled executable.

## Usage

1. Test each strategy by allocating and freeing blocks of different sizes.
2. Monitor the behavior of the memory allocator under each strategy, particularly focusing on external fragmentation.
