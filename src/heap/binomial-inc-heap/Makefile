CC=gcc
CFLAGS=-std=c99 -g
LIBS=libheap.a
LIBDIR=../../../lib
INCDIR=../../../include

$(LIBS): $(LIBS)(heap.o)

heap.o: heap.c heap.h
	$(CC) -c -o heap.o heap.c $(CFLAGS) -I$(INCDIR)

install:
	cp $(LIBS) $(LIBDIR)/binomial-inc-heap/
	cp heap.h $(INCDIR)/binomial-inc-heap/

test:
	$(CC) -o heap_test heap_test.c -I$(INCDIR) -I$(INCDIR)/binomial-inc-heap -L$(LIBDIR)/binomial-inc-heap -lheap -g && \
	./heap_test

clean:
	rm -f *.o *.a heap_test
