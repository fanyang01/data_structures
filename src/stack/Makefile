INCLUDEDIR=../../include
LIBDIR=../../lib
LIB=list
MYLIB=libstack.a

$(MYLIB): $(MYLIB)(stack.o) $(MYLIB)(list.o)

list.o:
	ar xv $(LIBDIR)/liblist.a

stack.o: stack.c stack.h
	gcc -c -o stack.o stack.c -I$(INCLUDEDIR) -L$(LIBDIR) -l$(LIB)

install:
	cp stack.h $(INCLUDEDIR)/
	cp $(MYLIB) $(LIBDIR)/

test:
	cc -o stack_test stack_test.c -I$(INCLUDEDIR) -L$(LIBDIR) -lstack && \
	./stack_test

clean:
	rm -f *.o *.a stack_test
