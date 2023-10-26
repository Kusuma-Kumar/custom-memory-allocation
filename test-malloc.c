#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "my-malloc.h"

// printMessage and printNode functions are declared and linked from my-malloc

int main(int argc, char *argv[]) {
    void *ptr1;
    void *ptr2;
    void *ptr3;
    void *ptr4;
    void *ptr6;
    uint64_t * ptr5;
    
    if((ptr1 = malloc(2003)) == NULL) {
        printMessage("malloc could not allocate for ptr1");
        return -1;
    }else{
        printMessage("ptr1: \n");
        printNode((struct Block *)((uintptr_t)ptr1 - alignedSize(sizeof(struct Block))));
        printMessage("\n");
    }
    
    if((ptr2 = malloc(810)) == NULL) {
        // i want this to reuse to malloc1 pointer and split into another memory location
        printMessage("malloc could not allocate for ptr2");
        return -1;
    }else{
        printMessage("ptr2: \n");
        printNode((struct Block *)((uintptr_t)ptr2 - alignedSize(sizeof(struct Block))));
        printMessage("\n");
    }

    if((ptr3 = malloc(1)) == NULL) {
        printMessage("malloc could not allocate for ptr3");
        return -1;
    }else{
        printMessage("ptr3: \n");
        printNode((struct Block *)((uintptr_t)ptr3 - alignedSize(sizeof(struct Block))));
        printMessage("\n");
    }

    // testing malloc_usable_size , for pt3 i only requested one byte by the usable size should be 16 bytes as it has to be 16 byte aligned
    printMessage("malloc_usable_size for ptr3:\n");
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%lu\n", malloc_usable_size(ptr3));
    write(1, buffer, len);
    printMessage("\n");
    
    // if i free ptr3 ,ptr1 and then ptr2, then the three should combine
    free(ptr3);
    free(ptr1);
    free(ptr2);
    printMessage("\n");
    printAllNodes();
    printMessage("\n");

    // testing split, i want it to resuse the block that just combined but it should split this into two, the second being the amount that wasnt used
    if((ptr4 = malloc(1000)) == NULL) {
        printMessage("malloc could not allocate for ptr4");
        return -1;
    }else{
        printMessage("ptr4: \n");
        printNode((struct Block *)((uintptr_t)ptr4 - alignedSize(sizeof(struct Block))));
        printMessage("\n");
    }

    printMessage("\n");
    printAllNodes();
    printMessage("\n");

    if((ptr6 = malloc(1872)) == NULL) {
        printMessage("malloc could not allocate for ptr4");
        return -1;
    }else{
        printMessage("ptr6: \n");
        printNode((struct Block *)((uintptr_t)ptr6 - alignedSize(sizeof(struct Block))));
        printMessage("\n");
    }

    printMessage("\n");
    printAllNodes();
    printMessage("\n");

    // testing calloc on int values
    int n = 5;
    if ((ptr5 = (void*)calloc(n, sizeof(uint64_t))) == NULL) {
        printMessage("Calloc could not allocate for ptr5");
        return -1;
    }
    else {
        printMessage("In for loop testing calloc int values\n");
        for (int i = 0; i < n; ++i) {
            char buffer[32];
            int len = snprintf(buffer, sizeof(buffer), "%lu ", ptr5[i]);
            write(1, buffer, len);
        }
        printMessage("\n\n");
    }

    // testing calloc on double values
    double* doubleArray;
    if ((doubleArray = (double*)calloc(n, sizeof(double))) == NULL) {
        printMessage("Calloc could not allocate for doubleArray");
        return -1;
    } else {
        for (int i = 0; i < n; ++i) {
            char buffer[32];
            int len = snprintf(buffer, sizeof(buffer), "%lf ", doubleArray[i]);
            write(1, buffer, len);
        }
        printMessage("\n\n");
    }

    // testing calloc on structs
    struct MyStruct {
        char x;
        float y;
    };
    struct MyStruct* structArray;
    if ((structArray = (struct MyStruct*)calloc(n, sizeof(struct MyStruct))) == NULL) {
        printMessage("Calloc could not allocate for structArray");
        return -1;
    } else {
        // would it prin the ASCII of c or just c
        for (int i = 0; i < n; i++) {
            char buffer[64];
            int len = snprintf(buffer, sizeof(buffer), "x: %c, y: %f\n", (char)structArray[i].x, structArray[i].y);
            write(1, buffer, len);
        }
        printMessage("\n\n");
    }
    
    // testing realloc
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
    printMessage("\n");

    free(ptr4);
    free(ptr5);
    free(doubleArray);
    free(structArray);
    return 0;
}
