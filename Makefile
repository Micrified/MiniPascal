CC=gcc
CFLAGS=-O2 -Wall -Wunused-function 
all: scanner parser 
	${CC} ${CFLAGS} lex.yy.c mpascal.tab.c -ll -lm

scanner: mpascal.lex
	flex mpascal.lex
parser: mpascal.y
	bison -d -v mpascal.y
clean:
	rm -f mpascal.tab.c
	rm -f mpascal.tab.h
	rm -f lex.yy.c
	rm -f *.o
	rm -f *~
