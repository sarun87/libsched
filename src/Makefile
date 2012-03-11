#CC = ~/bin/gcc
LINKER_FLAGS = -static -ggdb -L .
COMPILE_FLAGS = -ggdb
OBJS = myqueue.o mysched.o

%.o:	%.c
	${CC} -c -o $@ $< $(COMPILE_FLAGS)

all:	clean a5
	
a5: $(OBJS)
	$(CC) mytest.c $(COMPILE_FLAGS) $(OBJS) mythread-new3.a $(LINKER_FLAGS) -o a5

clean:
	rm  -f *.x *.o
