CC=gcc
CFLAGS=-std=c99 -g
LIBS=libtree.a
LIBDIR=../../lib
INCDIR=../../include

$(LIBS): $(LIBS)(tree.o)

tree.o: tree.c tree.h
	$(CC) -c -o tree.o tree.c $(CFLAGS) -I$(INCDIR) -L$(LIBDIR)

install:
	cp $(LIBS) $(LIBDIR)
	cp tree.h $(INCDIR)

test:
	$(CC) -o tree_test tree_test.c -I$(INCDIR) -L$(LIBDIR) -ltree -g && \
	./tree_test

clean:
	rm -f *.o *.a tree_test
