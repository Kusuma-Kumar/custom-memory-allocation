#ifndef MY_MALLOC_H
#define MY_MALLOC_H

struct memoryBlock {
    size_t size;      
    struct memoryBlock *next;  
    int isFree;
};

void* malloc(size_t size);
void free(void *ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
size_t malloc_usable_size(void* ptr);
void* setNextAligned(struct memoryBlock* block, size_t size);
void printNode(struct memoryBlock *block);
void printMessage(const char *message);
void split(struct memoryBlock* block, size_t size);
size_t alignedSize(size_t size);
void printAllNodes();

#endif