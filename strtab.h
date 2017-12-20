#if !defined(STRTAB_H)
#define STRTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    ***************************************************************************
    *                               String Table                              *
    * AUTHORS: Charles Randolph, Joe Jones.                                   *
    * SNUMBERS: s2897318, s2990652.                                           *
    ***************************************************************************
*/

/*
********************************************************************************
*                             String Table Routines                            *
********************************************************************************
*/

/* Initializes the string table */
void initStringTable ();

/* Returns index of installed identifier. If not yet in table, it is created. */
unsigned installId (const char *identifier);

/* Returns pointer to identifier lexeme at given index in the string table */
const char *identifierAtIndex (unsigned id);

/* Frees the string table */
void freeStringTable ();

/* Debug Method: Prints state of the table. */
void printStringTable();

#endif