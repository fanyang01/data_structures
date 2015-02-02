INCLUDEDIR=../../include
LIBDIR=../../lib
LIB=list
MYLIB=libqueue.a

$(MYLIB): $(MYLIB)(queue.o) $(MYLIB)(list.o)

list.o:
	ar xv $(LIBDIR)/liblist.a

queue.o: queue.c queue.h
	gcc -c -o queue.o queue.c -I$(INCLUDEDIR) -L$(LIBDIR) -l$(LIB)

install:
	cp queue.h $(INCLUDEDIR)/
	cp $(MYLIB) $(LIBDIR)/

clean:
	rm *.o
	rm *.a
