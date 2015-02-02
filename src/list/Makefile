MYLIBS=liblist.a
LIBDIR=../../lib/
INCDIR=../../include/
CC=gcc
CFLAGS=-std=c99 -g

$(MYLIBS): $(MYLIBS)(list.o)

list.o: list.c list.h
	$(CC) $(CFLAGS) -c -o list.o list.c

install:
	cp $(MYLIBS) $(LIBDIR)
	cp *.h $(INCDIR)

clean:
	rm *.o
	rm *.a
