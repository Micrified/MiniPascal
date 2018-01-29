CC=gcc
CFLAGS=-O2 -Wall -Wunused-function 
all: mpc.c 
	${CC} ${CFLAGS} -g -o mpc mpc.c

subsystem:
	cd frontend && $(MAKE)
	cd backend && $(MAKE)

clean:
	rm -f mpascal.tab.c
	rm -f mpascal.tab.h
	rm -f lex.yy.c
	rm -f *.o
	rm -f *~
	rm -rf *.dSYM
	rm -f *.output
	rm -f *.out
	rm -f mpc