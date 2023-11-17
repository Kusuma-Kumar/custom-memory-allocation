#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

struct memoryBlock {
    size_t size;      
    struct memoryBlock *next;  
    int isFree;
};

void printNode(struct memoryBlock *block);
void printMessage(const char *message);
void printAllNodes();

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
        if ((head = (struct memoryBlock *)sbrk(SBRK_REQUEST + STRUCT_SIZE + alignedSize(size))) == (void *) -1) {
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
            
            // call split only when its useful to split
            // Free the unused chunk so it is more efficent as user can use it later on 
            if (current->size >= alignedSize(size) + STRUCT_SIZE + ALIGNMENT) {
                struct memoryBlock* newBlock = (struct memoryBlock*)((uintptr_t)current + STRUCT_SIZE + alignedSize(size));
                newBlock->size = current->size - alignedSize(size) - STRUCT_SIZE;
                newBlock->next = current->next;
                newBlock->isFree = 1;

                current->size = alignedSize(size);
                current->next = newBlock;
                // freeing newBlock so it can be merged with free blocks close by
                free(newBlock);
            }
            
            return (void *)((uintptr_t)current + STRUCT_SIZE);
        }
        lastMemoryBlock = current;
        current = current->next;
    }

    // If no available block is found, request more memory
    struct memoryBlock *newBlock;
    if ((newBlock = (struct memoryBlock *)sbrk(SBRK_REQUEST + STRUCT_SIZE + alignedSize(size))) == (void *) -1) {
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

    if (blockToResize->size == alignedSize(size)) {
        // the blockToResize is already the requested size
        return ptr;
    }
    
    if(blockToResize->size > alignedSize(size)) {
        // The existing block is large enough so split it and return the pointer of the block which contains only the requested size
        struct memoryBlock* newBlock = (struct memoryBlock*)((uintptr_t)blockToResize + STRUCT_SIZE + alignedSize(size));
        newBlock->size = blockToResize->size - alignedSize(size) - STRUCT_SIZE;
        newBlock->next = blockToResize->next;
        newBlock->isFree = 1;

        blockToResize->size = alignedSize(size);
        blockToResize->next = newBlock;
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
    // added memory is not initialized, from man pages
    size_t copySize = (blockToResize->size < size) ? blockToResize->size : size;
    memcpy(newPtr, ptr, copySize);
        
    // Free the old block
    free(ptr);
    
    return newPtr;
}

size_t malloc_usable_size(void *ptr) {
    // my memoryBlock->size contains the usable size which is aligned (does not include metadata)
    if(ptr == NULL) {
        return 0;
    }
    
    struct memoryBlock *blockSize = (struct memoryBlock *)((uintptr_t)ptr - STRUCT_SIZE);
    
    return blockSize->size;
}

size_t alignedSize(size_t size) {
    // function to align any size request given according to ALIGNMENT value
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