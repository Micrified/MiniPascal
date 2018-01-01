CC=gcc
CFLAGS=-O2 -Wall -Wunused-function 
all: scanner parser debug.h debug.c colors.h mptypes.h strtab.h numtab.h symtab.h semantics.h 
	${CC} ${CFLAGS} lex.yy.c mpascal.tab.c strtab.c numtab.c symtab.c semantics.c -ll -lm

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
