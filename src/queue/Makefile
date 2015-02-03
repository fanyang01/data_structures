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

test:
	cc -o queue_test queue_test.c -I$(INCLUDEDIR) -L$(LIBDIR) -lqueue && \
	./queue_test

clean:
	rm -f *.o *.a queue_test
