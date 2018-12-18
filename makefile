OBJS = da.o cda.o stack.o queue.o integer.o string.o real.o scanner.o tnode.o bst.o gst.o rbt.o trees.o
OOPTS = -Wall -Wextra -g -c
LOPTS = -Wall -Wextra -g

all : trees

trees : $(OBJS)
		gcc $(LOPTS) $(OBJS) -o trees

da.o : da.c da.h
		gcc $(OOPTS) da.c

cda.o : cda.c cda.h
		gcc $(OOPTS) cda.c

stack.o : stack.c stack.h
		gcc $(OOPTS) stack.c

queue.o : queue.c queue.h
		gcc $(OOPTS) queue.c

integer.o : integer.c integer.h
		gcc $(OOPTS) integer.c

string.o : string.c string.h
		gcc $(OOPTS) string.c

real.o : real.c real.h
		gcc $(OOPTS) real.c

scanner.o : scanner.c scanner.h
		  gcc $(OOPTS) scanner.c

trees.o : trees.c
		gcc $(OOPTS) trees.c

tnode.o : tnode.c tnode.h
		gcc $(OOPTS) tnode.c

bst.o : bst.c bst.h
		gcc $(OOPTS) bst.c

gst.o : gst.c gst.h
		gcc $(OOPTS) gst.c

rbt.o : rbt.c rbt.h
		gcc $(OOPTS) rbt.c

test : trees
		./trees -g corpus commands

valgrind : trees
		valgrind ./trees

clean	:
		rm -f $(OBJS) trees
		rm -f *.o