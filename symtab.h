#if !defined(SYMTAB_H)
#define SYMTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strtab.h"

/*
    ***************************************************************************
    *                              Symbol Table                               *
    * AUTHORS: Charles Randolph, Joe Jones.                                   *
    * SNUMBERS: s2897318, s2990652.                                           *
    ***************************************************************************
*/


/*
********************************************************************************
*                            External Data Structures                          *
********************************************************************************
*/

/* ID Entry: Stores information about identifer */
typedef struct {
    unsigned id;    // Identifier index in string table.
    unsigned tt;    // Identifier token type.
} IdEntry;

/*
********************************************************************************
*                               IdEntry Routines                               *
********************************************************************************
*/

/* Returns nonzero if a is the same entry as b */
int equalsID (IdEntry a, IdEntry b);

/* Convenient initializer for IdEntry. */
IdEntry newIDEntry (unsigned id, unsigned tt);

/*
********************************************************************************
*                               Table Routines                                 *
********************************************************************************
*/

/* Sets the current table level or scope */
void setLevel (unsigned lvl);

/* Returns pointer to IdEntry if in table. Else NULL */
IdEntry *tableContains (const char *identifier);

/* Inserts a new IdEntry into the table. Returns pointer to entry. */
IdEntry *installEntry (IdEntry entry);

/* Removes all entries in the given level */
void freeTableLevel (int lvl);


#endif