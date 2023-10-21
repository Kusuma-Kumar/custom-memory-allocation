#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

struct Block {
    size_t size;      
    struct Block *next;  
    int isFree;      
};

// Define a pointer to the first block
static struct Block *head = NULL;

void printError(const char *);
void initialize();
void* malloc(size_t size);
void free(void *ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
size_t malloc_usable_size(void* ptr);
void printNode(const struct Block *block);



void initialize() {
    struct Block *initialBlock;
    
    if ((initialBlock = (struct Block *)sbrk(10)) == (void *)-1) {
        printError("Error: sbrk failed to allocate memory.\n");
        return;
    }

    initialBlock->size = 10;
    initialBlock->next = NULL;
    initialBlock->isFree = 1;
    head = initialBlock;
    printError("All fields initialized\n");
    printNode(head);
}




void* malloc(size_t size) {
    if(head == NULL){
        initialize();
    }
    
    struct Block* current = head;

    while (current != NULL) {
        if (current->isFree && current->size >= size) {
            current->isFree = 0; 
            return (void*)(current + sizeof(struct Block));
        }
        current = current->next;
    }
    // If no suitable block is found, request more memory using sbrk
    struct Block* newBlock;

    if ((newBlock = (struct Block*)sbrk(10)) == (void*)-1) {
        printError("sbrk did not give more data in malloc function\n");
        return NULL; 
    }

    newBlock->size = 10;
    newBlock->isFree = 0;
    newBlock->next = NULL;

    if (head->next == NULL) {
        head->next = newBlock;
    }

    return (void*)(newBlock + sizeof(struct Block));
    // can i also use pointer aritmetic to return (void*)(newBlock + 1);
}




void free(void *ptr) {
    // ptr has already been freed, undefined behavior occurs.  If ptr is NULL, no operation is performed.
    if (ptr == NULL) {
        return; 
    }
    // struct Block *blockToFree = (struct Block *)(uintptr_t)ptr - sizeof(struct Block);
    struct Block *blockToFree = (struct Block *)(uintptr_t)ptr;
    blockToFree->isFree = 1;
}




void* calloc(size_t nmemb, size_t size) {
    size_t totalSize = nmemb * size;
    void* ptr;
     
    if ((ptr = malloc(totalSize)) != NULL) {
        memset(ptr, 0, totalSize); // Initialize the allocated memory to zero
    }else{
        printError("Malloc didnt succeed in calloc\n");
    }
    return ptr;
}




void* realloc(void* ptr, size_t size) {
    // The realloc() function changes the size of the memory block pointed  to by  ptr to size bytes.
}




size_t malloc_usable_size(void* ptr) {
    
}




void printError(const char *message) {
    write(1, message, strlen(message));  // Write to stdout (fd = 1)
}




void printNode(const struct Block *block) {
    if (block != NULL) {
        size_t size = block->size;
        int isFree = block->isFree;
        char newline = '\n';

        write(STDOUT_FILENO, "Block Size: ", 12);
        write(STDOUT_FILENO, &size, sizeof(size_t));
        write(STDOUT_FILENO, &newline, 1);

        write(STDOUT_FILENO, "Next block\n", 11);

        write(STDOUT_FILENO, "Is Free: ", 9);
        write(STDOUT_FILENO, &isFree, sizeof(int));
        write(STDOUT_FILENO, &newline, 1);
    }
}




