#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "my-malloc.h"

// Define a global pointer to the first block
static struct Block *head = NULL;

void* malloc(size_t size) {
    // initialize head as beggining of arraylist with stuct values and first malloc request
    if (head == NULL) {
        if ((head = (struct Block*)sbrk(2048)) == (void *) -1) {
            printMessage("Error: sbrk failed to allocate memory.\n");
            return NULL;
        }
        head->size = size;
        head->next = setNextAligned(head, size);
        head->isFree = 0;
        printNode(head);
        return (void*)((uintptr_t)head + sizeof(struct Block));
    }
    // loop through structs to see if you have an empty position that has been freed
    struct Block* current = head;
    while(current->next != NULL){
        if (current->isFree == 1 && current->size >= size) {
            current->isFree = 0;
            return (void*)((uintptr_t)current + sizeof(struct Block));
        }
        current = current->next;
    }
    // if no position is found sbrk more bytes 
    struct Block* newBlock = current;
    void* temp;
    if ((temp = (void*) sbrk(2048)) == (void *) -1) {
        printMessage("Error: sbrk failed to allocate memory.\n");
        return NULL;
    }
    newBlock->size = size;
    newBlock->next = setNextAligned(newBlock, size);
    newBlock->isFree = 0;
    printNode(newBlock);
    return (void*)((uintptr_t)newBlock + sizeof(struct Block));
}

void free(void *ptr) {
    // ptr has already been freed, undefined behavior occurs.  If ptr is NULL, no operation is performed.
    if (ptr == NULL) {
        return; 
    }
    struct Block *blockToFree = (struct Block *)((uintptr_t)ptr - sizeof(struct Block));
    blockToFree->isFree = 1;
    printNode(blockToFree);
}

void* calloc(size_t nmemb, size_t size) {
    size_t totalSize = nmemb * size;
    void* ptr;
     
    if ((ptr = malloc(totalSize)) != NULL) {
        memset(ptr, 0, totalSize); // Initialize the allocated memory to zero
    }else{
        printMessage("Malloc didnt succeed in calloc\n");
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    // The realloc() function changes the size of the memory block pointed  to by  ptr to size bytes.
}

size_t malloc_usable_size(void* ptr) {
    
}

void printMessage(const char *message) {
    write(1, message, strlen(message));  // Write to stdout (fd = 1)
}

void printNode(struct Block *block) {
    if (block != NULL) {
        char buffer[256];
        // Convert to strings before writing
        snprintf(buffer, sizeof(buffer), "Block Size: %zu\n", block->size);
        write(STDOUT_FILENO, buffer, strlen(buffer));

        snprintf(buffer, sizeof(buffer), "Next block: %p\n", (void*)(block->next));
        write(STDOUT_FILENO, buffer, strlen(buffer));

        snprintf(buffer, sizeof(buffer), "Is Free: %d\n", block->isFree);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }
}

void* setNextAligned(struct Block* block, size_t size) {
    size_t alignment = 16;
    size_t padding = (alignment - ((uintptr_t)block % alignment)) % alignment;
    return (struct Block*)((uintptr_t)block + sizeof(struct Block) + size + padding);
}