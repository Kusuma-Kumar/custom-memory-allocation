#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *message;
    void *ptr1;
    if((ptr1 = malloc(20)) == NULL){
        message = "malloc could not allocate for ptr1";
        write(1, message, strlen(message));
        return -1;
    }else{
        write(1, ptr1, sizeof(void*));
    }
    void *ptr2;
    if((ptr2 = malloc(30)) == NULL){
        message = "malloc could not allocate for ptr3";
        write(1, message, strlen(message));
        return -1;
    }else{
        write(1, ptr2, sizeof(void*));
    }
    

    free(ptr1);

    return 0;
}
