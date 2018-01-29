CC=gcc
CFLAGS=-O2 -Wall -Wunused-function 
all: scanner parser mpio.c mpio.h irgen.h irgen.c symtab.h symtab.c strtab.h strtab.c numtab.h numtab.c mptypes.h mptypes.c
	${CC} ${CFLAGS} -g lex.yy.c mpascal.tab.c mpio.c irgen.c symtab.c strtab.c numtab.c mptypes.c -ll -lm

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
	rm -rf *.dSYM
	rm -f *.output
	rm -f *.out