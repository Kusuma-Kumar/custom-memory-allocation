#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include "my-malloc.h"

#define ALIGNMENT 16
#define SBRK_REQUEST 2048

size_t alignedSize(size_t size);
#define STRUCT_SIZE alignedSize(sizeof(struct memoryBlock))

// Define a global pointer to the first block
static struct memoryBlock *head = NULL;

void *malloc(size_t size) {
    if(size <= 0) {
        return NULL;
    }
    
    if (head == NULL) {
        // initial allocation of head as the beginning of array list
        if ((head = (struct memoryBlock *)sbrk(SBRK_REQUEST)) == (void *) -1) {
            // Error: sbrk failed to allocate memory
            return NULL;
        }
        head->size = alignedSize(size);
        head->isFree = 0;
        head->next = NULL;
        return (void *)((uintptr_t)head + STRUCT_SIZE);
    }
    
    // loop through structs to see if you have an empty position that has been freed
    struct memoryBlock *current = head;
    // Storing the last Node of the linked list to update the next pointer of the last block to point to this new block. 
    // Prevents me from having to loop through again if I do not have a free block i can use
    struct memoryBlock *lastMemoryBlock = head;
    
    while (current != NULL) {
        if (current->isFree == 1 && current->size >= size) {
            current->isFree = 0;
            // Free the unused chunk so it is more efficent as I can use it later on 
            // splitAndFreeUnused(current, size);
            split(current, size);
            return (void *)((uintptr_t)current + STRUCT_SIZE);
        }
        lastMemoryBlock = current;
        current = current->next;
    }

    // If no available block is found, request more memory
    struct memoryBlock *newBlock;
    if ((newBlock = (struct memoryBlock *)sbrk(SBRK_REQUEST)) == (void *) -1) {
        // Error: sbrk failed to allocate memory
        return NULL;
    }

    newBlock->size = alignedSize(size);
    newBlock->next = NULL;
    newBlock->isFree = 0;

    // Update the next pointer of the last block to point to the new block to keep them connected
    lastMemoryBlock->next = newBlock;
    return (void *)((uintptr_t)newBlock + STRUCT_SIZE);
}

void free(void *ptr) {
    // ptr has already been freed, undefined behavior occurs.  If ptr is NULL, no operation is performed
    if (ptr == NULL) {
        return; 
    }
    struct memoryBlock *blockToFree = (struct memoryBlock *)((uintptr_t)ptr - STRUCT_SIZE);
    blockToFree->isFree = 1;

   // Start the loop from the head and merge consecutive free blocks
   // in case user freed one right next to another free memoryblock or if user freed one in between two free memoryblocks
    struct memoryBlock *current = head;
    while (current != NULL) {
        if (current->isFree == 1 && current->next != NULL && current->next->isFree == 1) {
            current->size += current->next->size + STRUCT_SIZE;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void *calloc(size_t nmemb, size_t size) {
    if(nmemb <= 0 || size <= 0) {
        // nmemb or size is  0
        return NULL;
    }
    
    size_t totalSize = nmemb * size;
    // checking for size_t overflow on multiplication, we already know nmeb!= 0 from the first if condition
    if (totalSize / nmemb != size) {
        return NULL;
    }
    
    void *ptr;
    if ((ptr = malloc(totalSize)) == NULL) {
        // Malloc failed in calloc
        return NULL;
    }
    
    // Initialize the allocated memory to zero
    memset(ptr, 0, totalSize); 
    
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    
    if(size <= 0) {
        free(ptr);
        return NULL;
    }
    
    struct memoryBlock *blockToResize = (struct memoryBlock *)((uintptr_t)ptr - STRUCT_SIZE);

    if (blockToResize->size == size) {
        // the blockToResize is already the requested size
        return ptr;
    }
    
    if(blockToResize->size > size) {
        // The existing block is large enough so split it and return a the pointer of the block which contains only the requested size
        // splitAndFreeUnused(blockToResize, size);
        split(blockToResize, size);
        return ptr;
    } 
    
    // When the current block will not be able to handle our size request
    // Allocate a new block of the requested size with the same data and return the new ptr
    void *newPtr;   
    if ((newPtr = malloc(size)) == NULL) {
        // Malloc failed in realloc
        return NULL;
    } 
    
    // Copy data from the old block to the new block and free the old block
    // added memory is not initialized
    size_t copySize = (blockToResize->size < size) ? blockToResize->size : size;
    memcpy(newPtr, ptr, copySize);
        
    // Free the old block
    free(ptr);
    
    return newPtr;
}

size_t malloc_usable_size(void *ptr) {
    if(ptr == NULL) {
        return 0;
    }
    
    struct memoryBlock *blockSize = (struct memoryBlock *)((uintptr_t)ptr - STRUCT_SIZE);
    
    return blockSize->size;
}

void split(struct memoryBlock *blockToSplit, size_t size) {
    // if I am reassigning a previous allocation, and the current request is alteast ALIGNMENT (16 byte) + sizeof(struct memoryBlock)
    // I can store another malloc of a value between 1 and 16 here to save space, if not there is no use of splitting
    // since I am splitting this block into two, i have to also initialize values for my next struct which is current->next
    // initialNextBlock to keep track of the next pointer so i can reassign it to the the second block created during split
    if((alignedSize(size) + STRUCT_SIZE + ALIGNMENT) < blockToSplit->size ) {
        struct memoryBlock *initialNextBlock = blockToSplit->next;
        
        blockToSplit->next = (struct memoryBlock *)((uintptr_t)blockToSplit + STRUCT_SIZE + alignedSize(size));
        blockToSplit->next->size = blockToSplit->size - alignedSize(size) - STRUCT_SIZE;
        blockToSplit->next->isFree = 1;
        
        if(initialNextBlock == NULL) {
            // This happens only if the list has only one element which you are splitting right now or if the memoryblock being split is the last block in list
            blockToSplit->next->next = NULL;
        } else {
            blockToSplit->next->next = initialNextBlock;
        }
        
        blockToSplit->size = alignedSize(size);
        blockToSplit->isFree = 0;
    }
}

size_t alignedSize(size_t size) {
    if (size % ALIGNMENT == 0) {
        return size;
    } else {
        return ((size / ALIGNMENT) + 1) * ALIGNMENT;
    }
}

void printAllNodes() {
    printMessage("Printing all nodes\n");
    struct memoryBlock *current = head;

    while (current != NULL) {
        printNode(current);
        current = current->next;
    }
}

void printMessage(const char *message) {
    write(1, message, strlen(message));  // Write to stdout (fd = 1)
}

void printNode(struct memoryBlock *block) {
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