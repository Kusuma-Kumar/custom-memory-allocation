Authors: Kusuma Kumar

Bugs: 

Resources: https://stackoverflow.com/questions/7788934/how-to-convert-char-pointer-into-char-in-c-open-vms, https://stackoverflow.com/questions/9367987/what-is-the-difference-between-the-following-code-and-why-this-type-casting-is-u
https://stackoverflow.com/questions/1815367/catch-and-compute-overflow-during-multiplication-of-two-large-integers

Project Description:
This project involves the creation of a custom memory allocation library that replaces standard functions like malloc, free, calloc, realloc, and malloc_usable_size. The implementation utilizes system calls like brk and sbrk to request memory from the kernel efficiently. The library is dynamically linked and loaded before the standard libc library, influencing the behavior of memory-related functions in a running process.

Features
Memory Allocation: Implements functions malloc, free, calloc, realloc, and malloc_usable_size with behavior conforming to the respective manpages.

Library Loading: Utilizes LD_PRELOAD to load the custom library before libc, ensuring that the custom implementations override the standard ones during program execution.

Debugging with GDB: Facilitates debugging by using GDB with the LD_PRELOAD trick, allowing specific programs to be debugged with the custom library.

Mechanics
System Calls/Functions: Uses system calls brk, sbrk, and library functions memset for efficient memory management.

Static Keyword: Utilizes the static keyword to prevent the global variable from being exported and conflicting with other symbols in the process.

Compiling Shared Library: Compiles the library as a shared object using the GCC command with appropriate flags.

Usage
Clone the repository:

Build the shared library using the provided Makefile:

bash
Copy code
make
Run programs with the custom memory allocation library:

bash
Copy code
LD_PRELOAD=./my-malloc.so <program>