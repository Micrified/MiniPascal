#if !defined(SEMANTICS_H)
#define SEMANTICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/*
********************************************************************************
*                                 Prototypes                                   *
********************************************************************************
*/

/* Returns token type of identifier. If no entry, one is made with type `tt`. */
unsigned getTypeElseInstall (const char *identifier, unsigned tt);

/* Resolves an expected class to a primitive return value.
 * 1. Extracts IdEntry for given `identifier`.
 * 2. Verifies token-type is of correct class using mask.
 * 3. Resolves class to return literal.
*/
unsigned resolveClass (const char *identifier,  unsigned mask);

/* Returns the appropriate type for an operation between token types.
 * Produces a warning if literals mismatch.
*/
unsigned resolveOperation (unsigned tt_a, unsigned tt_b);

#endif 