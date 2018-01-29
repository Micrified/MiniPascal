#include "numtab.h"

/*
********************************************************************************
*                    Symbolic Constants & Global Variables                     *
********************************************************************************
*/

#define NUMTAB_DEFAULT_SIZE     512

// Number table.
double *numTable;

// Points to the front of the concatenated string table.
unsigned np, numTableSize;

/*
********************************************************************************
*                         Internal Number Table Routines                       *
********************************************************************************
*/

static void resizeNumberTable (unsigned newSize) {
    if (newSize < numTableSize) {
        fprintf(stderr, "Error: numtab: Can't resize table to smaller size!\n");
        exit(EXIT_FAILURE);
    }

    if ((numTable = realloc(numTable, newSize * sizeof(double))) == NULL) {
        fprintf(stderr, "Error: numtab: Couldn't resize number table!\n");
        exit(EXIT_FAILURE);
    }
    numTableSize = newSize;
}

/*
********************************************************************************
*                           Number Table Routines                              *
********************************************************************************
*/

/* Initializes the number table */
void initNumberTable () {
    numTableSize = NUMTAB_DEFAULT_SIZE;
    if ((numTable = malloc(numTableSize * sizeof(double))) == NULL) {
        fprintf(stderr, "Error: numtab: Couldn't allocate table!\n");
        exit(EXIT_FAILURE);
    }
}

/* Installs given constant in the table and returns its index */
unsigned installNumber (double n) {
    if (np >= numTableSize) {
        resizeNumberTable(2 * numTableSize);
    }
    numTable[np++] = n;
    return (np - 1);
}

/* Returns pointer to constant at given index in the string table. */
double *numberAtIndex (unsigned vi) {
    if (vi >= np) {
        return NULL;
    }
    return numTable + vi;
}

/* Frees the number table */
void freeNumberTable () {
    free(numTable);
}

/* Debug Method: Prints state of the table. */
void printNumberTable() {
    printf("Size = %u\nHead = %u\nTable = [", numTableSize, np);
    for (int i = 0; i < np; i++) {
        printf("%.3f",numTable[i]);
        if (i < np - 1) {
            putchar(',');
        }
    }
    printf("]\n");
}
