#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "my-malloc.h"

int main(int argc, char *argv[]) {
    void *ptr1;
    void *ptr2;
    void *ptr3;
    void *ptr4;
    uint64_t * ptr5;
    if((ptr1 = malloc(2000)) == NULL){
        printMessage("malloc could not allocate for ptr1");
        return -1;
    }else{
        printMessage("ptr1: \n");
        printNode((struct Block *)((uintptr_t)ptr1 - sizeof(struct Block)));
    }
    free(ptr1);
    if((ptr2 = malloc(800)) == NULL){
        // does this have to split allocation?
        printMessage("malloc could not allocate for ptr2");
        return -1;
    }else{
        printMessage("ptr2: \n");
        printNode((struct Block *)((uintptr_t)ptr2 - sizeof(struct Block)));
    }
    if((ptr3 = malloc(600)) == NULL){
        printMessage("malloc could not allocate for ptr3");
        return -1;
    }else{
        printMessage("ptr3: \n");
        printNode((struct Block *)((uintptr_t)ptr3 - sizeof(struct Block)));
    }
    free(ptr3);
    if((ptr4 = malloc(1200)) == NULL){
        printMessage("malloc could not allocate for ptr4");
        return -1;
    }else{
        printMessage("ptr4: \n");
        printNode((struct Block *)((uintptr_t)ptr4 - sizeof(struct Block)));
    }
    int n = 5;
    if ((ptr5 = (void*)calloc(n, sizeof(uint64_t))) == NULL) {
        printMessage("Calloc could not allocate for ptr5");
        return -1;
    }
    else {
        printMessage("In for loop testing calloc values\n");
        for (int i = 0; i < n; ++i) {
            char buffer[32];
            int len = snprintf(buffer, sizeof(buffer), "%lu, ", ptr5[i]);
            write(1, buffer, len);
        }
        printMessage("\n");
    }
    int *ptr = (int *)malloc(sizeof(int)*2); 
    int *ptr_new; 
    *ptr = 10;  
    *(ptr + 1) = 20; 
    ptr_new = (int *)realloc(ptr, sizeof(int)*3); 
    *(ptr_new + 2) = 30; 
    printMessage("In for loop testing realloc values, adding third variable to original array\n"); 
    for (int i = 0; i < 3; ++i) {
        char buffer[32];
        int len = snprintf(buffer, sizeof(buffer), "%d", *(ptr_new + i));
        write(1, buffer, len);
        printMessage("\n");
    }
    uint8_t *test;
    test = (uint8_t *)malloc(sizeof(uint8_t)*4);
    printf("sizeof(test) = %ld\n",malloc_usable_size(test));
    // free(test); 
    // free(ptr2);
    // free(ptr4);
    // free(ptr5);
    return 0;
}
