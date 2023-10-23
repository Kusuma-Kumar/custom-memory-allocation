#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my-malloc.h"

int main(int argc, char *argv[]) {
    const char *message;
    void *ptr1;
    if((ptr1 = malloc(2000)) == NULL){
        message = "malloc could not allocate for ptr1";
        write(1, message, strlen(message));
        return -1;
    }else{
        // printNode(ptr1);
        char ptr1_str[20];  // Assuming 20 characters is sufficient for the pointer representation
        snprintf(ptr1_str, sizeof(ptr1_str), "%p", ptr1);
        write(1, "ptr1: ", 6);
        write(1, ptr1_str, strlen(ptr1_str));
        write(1, "\n", 1);
    }
    free(ptr1);
    void *ptr2;
    if((ptr2 = malloc(800)) == NULL){
        // does this have to split allocation?
        message = "malloc could not allocate for ptr3";
        write(1, message, strlen(message));
        return -1;
    }else{
        // printNode((struct Block*)ptr2);
        char ptr2_str[20]; 
        snprintf(ptr2_str, sizeof(ptr2_str), "%p", ptr2);
        write(1, "ptr2: ", 6);
        write(1, ptr2_str, strlen(ptr2_str));
        write(1, "\n", 1);
    }
    void *ptr3;
    if((ptr3 = malloc(1210)) == NULL){
        message = "malloc could not allocate for ptr3";
        write(1, message, strlen(message));
        return -1;
    }else{
        // printNode(ptr3);
        char ptr3_str[20];  
        snprintf(ptr3_str, sizeof(ptr3_str), "%p", ptr3);
        write(1, "ptr3: ", 6);
        write(1, ptr3_str, strlen(ptr3_str));
        write(1, "\n", 1);
    }
    // free(ptr2);
    free(ptr3);
    void *ptr4;
    if((ptr4 = malloc(1200)) == NULL){
        message = "malloc could not allocate for ptr4";
        write(1, message, strlen(message));
        return -1;
    }else{
        // printNode(ptr4);
        char ptr4_str[20];  // Assuming 20 characters is sufficient for the pointer representation
        snprintf(ptr4_str, sizeof(ptr4_str), "%p", ptr4);
        write(1, "ptr4: ", 6);
        write(1, ptr4_str, strlen(ptr4_str));
        write(1, "\n", 1);
    }

    return 0;
}
