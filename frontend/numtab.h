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

/*
********************************************************************************
*                            Number Table Routines                             *
********************************************************************************
*/

/* Initializes the number table */
void initNumberTable ();

/* Installs given constant in the table and returns its index */
unsigned installNumber (double n);

/* Returns pointer to constant at given index in the string table. */
double *numberAtIndex (unsigned vi);

/* Frees the number table */
void freeNumberTable ();

/* Debug Method: Prints state of the table. */
void printNumberTable();



#endif