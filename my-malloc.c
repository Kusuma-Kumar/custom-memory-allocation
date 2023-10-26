#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include "my-malloc.h"

// Define a global pointer to the first block
static struct Block *head = NULL;

void* malloc(size_t size) {
    // initialize head as beggining of arraylist with stuct values and first malloc request
    if(size == 0) {
        return NULL;
    }
    
    if (head == NULL) {
        if ((head = (struct Block*)sbrk(2048)) == (void *) -1) {
            printMessage("Error: sbrk failed to allocate memory\n");
            return NULL;
        }
        head->size = alignedSize(size);
        head->next = setNextAligned(head, size);
        head->isFree = 0;
        // printNode(head);
        return (void*)((uintptr_t)head + alignedSize(sizeof(struct Block)));
    }
    
    // loop through structs to see if you have an empty position that has been freed
    struct Block* current = head;
    while(current->next != NULL){
        if (current->isFree == 1 && current->size >= size) {
            current->isFree = 0;
            split(current, size);
            return (void*)((uintptr_t)current + alignedSize(sizeof(struct Block)));
        }
        current = current->next;
    }

    // if no position is found sbrk more bytes 
    struct Block* newBlock = current;
    void* temp;
    if ((temp = (void*) sbrk(2048)) == (void *) -1) {
        printMessage("Error: sbrk failed to allocate memory\n");
        return NULL;
    }
    newBlock->size = alignedSize(size);
    newBlock->next = setNextAligned(newBlock, size);
    newBlock->isFree = 0;
    // printNode(newBlock);
    
    return (void*)((uintptr_t)newBlock + alignedSize(sizeof(struct Block)));
}

void free(void *ptr) {
    // ptr has already been freed, undefined behavior occurs.  If ptr is NULL, no operation is performed
    if (ptr == NULL) {
        return; 
    }
    
    struct Block *blockToFree = (struct Block *)((uintptr_t)ptr - alignedSize(sizeof(struct Block)));
    blockToFree->isFree = 1;

   // Start the loop from the head and merge consecutive free blocks, in case i freed one right next to another free or if i freed one in between two frees
    struct Block *current = head;
    while (current != NULL) {
        while (current->isFree == 1 && current->next->isFree == 1) {
            current->size += current->next->size + alignedSize(sizeof(struct Block));
            current->next = current->next->next;
        }
        current = current->next;
    }
    
    printMessage("Freed a ptr \n");
    printNode(blockToFree);
}

void* calloc(size_t nmemb, size_t size) {
    if(nmemb == 0 || size == 0){
        printMessage("nmemb or size is  0\n");
        return NULL;
    }
    size_t totalSize = nmemb * size;
    void* ptr;
    if ((ptr = malloc(totalSize)) == NULL) {
        printMessage("Malloc failed in calloc\n");
    }else{
        memset(ptr, 0, totalSize); // Initialize the allocated memory to zero
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    // The realloc() function changes the size of the memory block pointed to by ptr to size bytes
    if (ptr == NULL) {
        return malloc(size);
    }
    if(size == 0 && ptr!= NULL) {
        free(ptr);
        return NULL;
    }
    struct Block *blockToResize = (struct Block *)((uintptr_t)ptr - alignedSize(sizeof(struct Block)));
    if (blockToResize->size >= size) {
        // The existing block is large enough
        split(blockToResize, size);
        return ptr;
    }else {
        // Allocate a new block of the requested size
        void *newPtr;
        if ((newPtr = malloc(size)) == NULL) {
             printMessage("Malloc failed in realloc\n");
        }else{
            // Copy data from the old block to the new block and free the old block
            size_t copySize = (blockToResize->size < size) ? blockToResize->size : size;
            memcpy(newPtr, ptr, copySize);
            // Free the old block
            free(ptr);
        }
        return newPtr;
    }
}

size_t malloc_usable_size(void* ptr) {
    if(ptr == NULL){
        return 0;
    }
    struct Block *blockSize = (struct Block *)((uintptr_t)ptr - alignedSize(sizeof(struct Block)));
    return blockSize->size;
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
    return (struct Block*)((uintptr_t)block + alignedSize(sizeof(struct Block)) + alignedSize(size));
}

void split(struct Block* block, size_t size){
    // if I am reassigning a previous allocation, and the current request is alteast 16 byte + sizeof(struct Block)
    // I can store another malloc of a value between 1 and 16 here to save space, if not there is no use of splitting
    // since I am splitting this block into two, i have to initialize values for my next struct which is current->next
    // change to calculate the least amount of data i can malloc to get the first automatic 16 byte aligned value
    printMessage("Splitting Node\n");
    if(alignedSize(size) + alignedSize(sizeof(struct Block)) + 16 < block->size ){
        block->next = setNextAligned(block, size);
        block->next->size = block->size - alignedSize(size) - alignedSize(sizeof(struct Block));
        block->next->isFree = 1;
        block->size = alignedSize(size);
        block->isFree = 0;
        block->next->next = setNextAligned(block->next, size);
        printMessage("Node1: \n");
        printNode(block);
        printMessage("Node2: \n");
        printNode(block->next);
    }
    printMessage("\n");
}

size_t alignedSize(size_t size) {
    if (size % 16 == 0) {
        return size;
    } else {
        return ((size / 16) + 1) * 16;
    }
}

void printAllNodes() {
    printMessage("Printing all nodes\n");
    struct Block *current = head;
    while (current->next != NULL) {
        printNode(current);
        current = current->next;
    }
}