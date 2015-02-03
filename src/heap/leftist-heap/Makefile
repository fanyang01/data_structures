CC=gcc
CFLAGS=-std=c99 -g
LIBS=libheap.a
LIBDIR=../../../lib/leftist-heap
INCDIR=../../../include/leftist-heap

$(LIBS): $(LIBS)(heap.o)

heap.o: heap.c heap.h
	$(CC) -c -o heap.o heap.c $(CFLAGS)

install:
	cp $(LIBS) $(LIBDIR)
	cp heap.h $(INCDIR)

test:
	$(CC) -o heap_test heap_test.c -I$(INCDIR) -L$(LIBDIR) -lheap -g && \
	./heap_test

clean:
	rm -f *.o *.a heap_test
