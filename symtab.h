#if !defined(SYMTAB_H)
#define SYMTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mptypes.h"
#include "strtab.h"
#include "debug.h"

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

// Indicates that a search for an IdEntry may traverse lower scope-levels.
#define SYMTAB_SCOPE_ALL   -1

/* Routine ID-Entry (Function, Procedure) */
typedef struct {     
    unsigned argc;  // Argument count.
    void **argv;    // Vector of IdEntry table pointers.
} IdData;

/* IdEntry: Symbol Table Entry */
typedef struct {
    unsigned id;        // Identifier index in string table.
    unsigned tc;        // Type-class: Routine / Vector / Scalar
    unsigned tt;        // Token-type.
    unsigned rf;        // Referenced boolean flag.
    IdData data;        // Type data.
} IdEntry;

/*
********************************************************************************
*                           Table IdEntry Prototypes                           *
********************************************************************************
*/

/* Copies an IdEntry and returns a pointer. Exits on error. */
IdEntry *copyIdEntry (IdEntry *entry);

/* Returns pointer to IdEntry if it exists in the symbol table. Else returns
 * NULL. Entries may have the same identifiers so long as their class is unique.
 * 
 * Parameter `scope` defines the search scope. If a positive integer, scope
 *  at literal value of integer is searched. Otherwise all descending table
 *  levels are traversed.
*/ 
IdEntry *containsIdEntry (unsigned id, unsigned tc, int scope);


/* Allocates and installs new IdEntry into the symbol table at current scope.
 * Returns pointer if successful. Duplicate entry must not already exist. 
*/
IdEntry *installIdEntry (unsigned id, unsigned tc, unsigned tt);

/*
********************************************************************************
*                           Table Control Prototypes                           *
********************************************************************************
*/

/* Increments table scope level */
void incrementTableScope (void);

/* Decrements table scope level: Frees memory in current scope */
void decrementTableScope (void);

/* Returns the table scope level */
unsigned currentTableScope (void);

/* Frees all allocated entires in all table levels */
void freeSymbolTables (void);

/* Prints all symbol table entires */
void printSymbolTables (void);

#endif