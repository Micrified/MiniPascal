#include "strtab.h"

/*
********************************************************************************
*                    Symbolic Constants & Global Variables                     *
********************************************************************************
*/

#define STRTAB_DEFAULT_SIZE     512
#define MAX(a,b)                ((a) > (b) ? (a) : (b))

// String table.
char *strTable;

// Points to the front of the concatenated string table.
unsigned sp, strTableSize;

/*
********************************************************************************
*                       Internal String Table Routines                         *
********************************************************************************
*/

static void resizeStringTable (unsigned newSize) {
    if (newSize < strTableSize) {
        fprintf(stderr, "Error: strtab: Can't resize table to smaller size!\n");
        exit(EXIT_FAILURE);
    }

    if ((strTable = realloc(strTable, newSize * sizeof(char))) == NULL) {
        fprintf(stderr, "Error: strtab: Couldn't resize string table!\n");
        exit(EXIT_FAILURE);
    }
    strTableSize = newSize;
}


/*
********************************************************************************
*                           String Table Routines                              *
********************************************************************************
*/

/* Initializes the string table */
void initStringTable () {
    strTableSize = STRTAB_DEFAULT_SIZE;
    if ((strTable = malloc(strTableSize * sizeof(char))) == NULL) {
        fprintf(stderr, "Error: strtab: Couldn't allocate table!\n");
        exit(EXIT_FAILURE);
    }
}

/* Returns index of installed identifier. If not yet in table, it is created. */
unsigned installId (const char *identifier) {
    int p = 0, len = strlen(identifier);

    // Try to locate identifier.
    while (p < sp) {
        if (strcmp(strTable + p, identifier) == 0) {
            return p;
        }
        p += strlen(strTable + p) + 1;
    }

    // Must not exist, resize if necessary and install it.
    if (p + len + 1 > strTableSize) {
        resizeStringTable(MAX(strTableSize * 2, p + len + 1));
    }
    strcpy(strTable + p, identifier);
    sp = p + len + 1;
    
    return p;
}

/* Returns pointer to identifier lexeme at given index in the string table */
const char *identifierAtIndex (unsigned id) {
    if (id >= sp) {
        fprintf(stderr, "Error: strtab: Given index out of bounds!\n");
        exit(EXIT_FAILURE);
    }
    return strTable + id;
}

/* Frees the string table */
void freeStringTable () {
    free(strTable);
}

/* Debug Method: Prints state of the table. */
void printStringTable() {
    printf("Size = %u\nHead = %u\nTable = [", strTableSize, sp);
    for (int i = 0; i < sp; i++) {
        if (strTable[i] == '\0') {
            putchar('|');
        } else {
            putchar(strTable[i]);
        }
    }
    printf("]\n");
}
