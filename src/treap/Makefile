CC=gcc
CFLAGS=-std=c99 -g
LIBS=libtreap.a
LIBDIR=../../lib
INCDIR=../../include

$(LIBS): $(LIBS)(treap.o)

treap.o: treap.c treap.h
	$(CC) -c -o treap.o treap.c $(CFLAGS)

install:
	cp $(LIBS) $(LIBDIR)
	cp treap.h $(INCDIR)

test:
	$(CC) -o treap_test treap_test.c -I$(INCDIR) -L$(LIBDIR) -ltreap -g && \
	./treap_test

clean:
	rm -f *.o *.a treap_test
