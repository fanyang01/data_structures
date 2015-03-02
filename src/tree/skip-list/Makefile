CC=gcc
CFLAGS=-std=c99 -g
LIBS=libskiplist.a
LIBDIR=../../../lib
INCDIR=../../../include

$(LIBS): $(LIBS)(skiplist.o)

skiplist.o: skiplist.c skiplist.h
	$(CC) -c -o skiplist.o skiplist.c $(CFLAGS)

install:
	mkdir -p $(LIBDIR)/skip-list $(INCDIR)/skip-list
	cp $(LIBS) $(LIBDIR)/skip-list
	cp skiplist.h $(INCDIR)/skip-list

test:
	$(CC) -o skiplist_test skiplist_test.c -L$(LIBDIR)/skip-list -lskiplist -g && \
	./skiplist_test

clean:
	rm -f *.o *.a skiplist_test
