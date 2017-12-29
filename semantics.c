#include "semantics.h"

#define MAX(a,b)        ((a) > (b) ? (a) : (b))

/*
********************************************************************************
*                                 Functions                                    *
********************************************************************************
*/

/* Returns token type of identifier. If no entry, one is made with type `tt`. */
unsigned getTypeElseInstall (const char *identifier, unsigned tt) {
    IdEntry *entry;

    // Check if entry exists, otherwise install with type `tt`.
    if ((entry = tableContains(identifier)) == NULL) {
        entry = installEntry(newIDEntry(installId(identifier), tt));
    }

    return entry->tt;
}

/* Resolves an expected class to a literal return value.
 * 1. Extracts IdEntry for given `identifier`.
 * 2. Verifies token-type is of correct class using mask.
 * 3. Resolves class to return literal.
*/
unsigned resolveClass (const char *identifier, unsigned mask) {
    IdEntry *entry;

    // Verify: IdEntry exists. 
    if ((entry = tableContains(identifier)) == NULL) {
        fprintf(stderr, "Error: %s is undefined!\n", identifier);
        exit(EXIT_SUCCESS);
    }

    // Verify: Token-Type is of expected class.
    if ((entry->tt & mask) == 0) {
        fprintf(stderr, "Error: %s is not of class %u!\n", identifier, mask);
        exit(EXIT_SUCCESS);
    }

    // Resolve class to primitive (integer = 0, real = 1).
    return (entry->tt % 2);
}

/* Returns the appropriate type for an operation between token types.
 * Produces a warning if literals mismatch.
*/
unsigned resolveOperation (unsigned tt_a, unsigned tt_b) {
    if (tt_a ^ tt_b) {
        fprintf(stderr, "Warning: Operation applied between mismatched types!\n");
    }

    /* Always type promote to float if mismatch exists */
    return MAX(tt_a, tt_b);
}