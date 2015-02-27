CC=gcc
CFLAGS=-std=c99 -g
LIBS=libtree.a
LIBDIR=../../../lib
INCDIR=../../../include

$(LIBS): $(LIBS)(tree.o)

tree.o: tree.c tree.h
	$(CC) -c -o tree.o tree.c $(CFLAGS)

install:
	mkdir -p $(LIBDIR)/avl-tree $(INCDIR)/avl-tree
	cp $(LIBS) $(LIBDIR)/avl-tree
	cp tree.h $(INCDIR)/avl-tree

test:
	$(CC) -o tree_test tree_test.c -L$(LIBDIR)/avl-tree -ltree -g && \
	./tree_test

clean:
	rm -f *.o *.a tree_test
