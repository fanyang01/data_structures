CC=gcc
CFLAGS=-std=c99 -g
LIBS=libheap.a
LIBDIR=../../../lib
INCDIR=../../../include

$(LIBS): $(LIBS)(heap.o) $(LIBS)(list.o)

heap.o: heap.c heap.h
	$(CC) -c -o heap.o heap.c $(CFLAGS) -I$(INCDIR) -L$(LIBDIR) -llist

list.o: $(LIBDIR)/liblist.a
	ar xv $(LIBDIR)/liblist.a

install:
	cp $(LIBS) $(LIBDIR)/fib-heap/
	cp heap.h $(INCDIR)/fib-heap/

test:
	$(CC) -o heap_test heap_test.c -I$(INCDIR) -I$(INCDIR)/fib-heap -L$(LIBDIR)/fib-heap -lheap -g && \
	./heap_test

clean:
	rm -f *.o *.a heap_test
