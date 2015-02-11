MYLIBS=libhash.a
LIBDIR=../../lib
INCDIR=../../include
CC=gcc
CFLAGS=-std=c99 -g

$(MYLIBS): $(MYLIBS)(hash.o)

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c -o hash.o hash.c

install:
	cp $(MYLIBS) $(LIBDIR)
	cp *.h $(INCDIR)

test:
	$(CC) -o hash_test hash_test.c -I$(INCDIR) -L$(LIBDIR) -lhash $(CFLAGS) && \
	./hash_test

clean:
	rm -f *.o *.a hash_test
