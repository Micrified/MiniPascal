#if !defined(NUMTAB_H)
#define NUMTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    ***************************************************************************
    *                               Number Table                              *
    * AUTHORS: Charles Randolph, Joe Jones.                                   *
    * SNUMBERS: s2897318, s2990652.                                           *
    ***************************************************************************
*/

/*
********************************************************************************
*                              Symbolic Constants                              *
********************************************************************************
*/

// Represents an invalid table number index.
#define NIL         -1      

/*
********************************************************************************
*                            Number Table Routines                             *
********************************************************************************
*/

/* Initializes the number table */
void initNumberTable ();

/* Installs given constant in the table and returns its index */
int installNumber (double n);

/* Returns pointer to constant at given index in the string table. */
double *numberAtIndex (int i);

/* Frees the number table */
void freeNumberTable ();

/* Debug Method: Prints state of the table. */
void printNumberTable();



#endif