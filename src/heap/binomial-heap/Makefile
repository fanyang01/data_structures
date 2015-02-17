CC=gcc
CFLAGS=-std=c99 -g
LIBS=libheap.a
LIBDIR=../../../lib
INCDIR=../../../include

$(LIBS): $(LIBS)(heap.o)

heap.o: heap.c heap.h
	$(CC) -c -o heap.o heap.c $(CFLAGS) -I$(INCDIR)

install:
	cp $(LIBS) $(LIBDIR)/binomial-heap/
	cp heap.h $(INCDIR)/binomial-heap/

test:
	$(CC) -o heap_test heap_test.c -I$(INCDIR) -I$(INCDIR)/binomial-heap -L$(LIBDIR)/binomial-heap -lheap -g && \
	./heap_test

sort:
	$(CC) -o sort sort.c -I$(INCDIR) -I$(INCDIR)/binomial-heap -L$(LIBDIR)/binomial-heap -lheap -g && \
	./sort

clean:
	rm -f *.o *.a heap_test sort
