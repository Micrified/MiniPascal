#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
***************************************************************************
*                          Mini Pascal Compiler                           *
* AUTHORS: Charles Randolph, Joe Jones.                                   *
* SNUMBERS: s2897318, s2990652.                                           *
***************************************************************************
*/

#define USAGE       "./mpc <InputFile> <OutputFile>\n"

#define MAXLINE     1000

char line[MAXLINE];

int main (int argc, const char *argv[]) {

    /* Verify correct number of arguments are provided. */
    if (argc != 3) {
        fprintf(stderr, USAGE);
        return EXIT_FAILURE;
    }

    /* Perform Semantic Analysis: Remove the "-c" flag to disable color */
    sprintf(line, "./frontend/a.out -c < %s", argv[1]);
    int verifiedSemantics = system(line);

    /* If successful, generate IR code. */
    if (verifiedSemantics == EXIT_SUCCESS) {
        sprintf(line, "./backend/a.out %s < %s", argv[2], argv[1]);
        system(line);
    } else {
        fprintf(stderr, "mpc: Compilation failed at semantic stage!\n");
    }

    return 0;
}