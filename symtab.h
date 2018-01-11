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
#define SYMTAB_SCOPE_ALL    -1

// Indicates that any IdEntry may be selected for a given id.
#define TC_ANY              -1

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

/* Returns pointer to IdEntry if in the symbol-table. Otherwise returns NULL.
 * An IdEntry is uniquely identified by combination (id, tc).
 * Parameters:
 * - id: The identifier-index.
 *
 * - tc: The token-class. If not a positive integer, first matching IdEntry
 *       with the given id is returned. Typically last installed IdEntry.
 *
 * - scope: The search scope. If not positive integer, all descending
 *          scope levels are traversed. Otherwise literal value used.
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