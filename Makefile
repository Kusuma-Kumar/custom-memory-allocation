CFLAGS = -g -Wall -pedantic 

.PHONY: all clean

all: my-malloc.so test-malloc

my-malloc.so: my-malloc.c
	gcc $(CFLAGS) -rdynamic -shared -fPIC -o my-malloc.so my-malloc.c

test-malloc: test-malloc.c my-malloc.so
	gcc $(CFLAGS) -o test-malloc test-malloc.c

gdb: all 
	gdb --args env LD_PRELOAD=./my-malloc.so ./test-malloc

strace: all
	strace -o test-malloc.strace ./test-malloc

clean:
	rm -f my-malloc.so test-malloc