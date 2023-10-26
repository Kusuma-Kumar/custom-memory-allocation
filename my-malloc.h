#ifndef MY_MALLOC_H
#define MY_MALLOC_H

struct Block {
    size_t size;      
    struct Block *next;  
    int isFree;
};

void* malloc(size_t size);
void free(void *ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
size_t malloc_usable_size(void* ptr);
void* setNextAligned(struct Block* block, size_t size);
void printNode(struct Block *block);
void printMessage(const char *message);
void split(struct Block* block, size_t size);
size_t alignedSize(size_t size);
void printAllNodes();

#endif