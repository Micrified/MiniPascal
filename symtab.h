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
*                 External Data Structures & Symbolic Constants                *
********************************************************************************
*/

/* ID Entry: Stores information about identifer */
typedef struct {
    unsigned id;    // Identifier index in string table.
    unsigned tt;    // Identifier token type.
    unsigned argc;  // Argument Count (for routines).
    void    **argv;  // Array of IdEntry pointers (for routines).
} IdEntry;

/*
********************************************************************************
*                               IdEntry Routines                               *
********************************************************************************
*/

/* Returns nonzero if a is the same entry as b */
int equalsID (IdEntry a, IdEntry b);

/* Convenient initializer for standard IdEntry. */
IdEntry newIDEntry (unsigned id, unsigned tt);

/* Frees an IdEntry. */
void freeIdEntry (IdEntry entry);

/*
********************************************************************************
*                               Table Routines                                 *
********************************************************************************
*/

/* Increments the current table level or scope */
void incrementScopeLevel (void);

/* Decrements the current table level or scope */
void decrementScopeLevel (void);

/* Returns pointer to IdEntry if in table. Else NULL */
IdEntry *tableContains (const char *identifier);

/* Inserts a new IdEntry into the table. Returns pointer to entry. */
IdEntry *installEntry (IdEntry entry);

/* Frees all allocated entries in the given level */
void freeTableLevelEntries (int lvl);

/* Frees all allocated entries */
void freeSymbolTableEntries (void); 

/* Debug Method: Print state of the symbol tables */
void printSymbolTables (void);


#endif