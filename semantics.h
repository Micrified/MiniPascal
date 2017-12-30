#if !defined(SEMANTICS_H)
#define SEMANTICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mptypes.h"
#include "symtab.h"

/*
********************************************************************************
*                               ExprType Routines                              *
********************************************************************************
*/

// TODO
// 1. Rewrite routines in semantics.h
// 2. Compile for expressions.
// 3. Test run.

/* Returns token type of identifier. If no entry, one is made with type `tt`. */
unsigned getTypeElseInstall (const char *identifier, unsigned tt);

/* Returns primitive token-type for expected type the given class.
 * 1. Throws undefined-type error if no IdEntry exists for given identifier.
 * 2. Throws unexpected-type error if IdEntry doesn't match expected class.
 * Resolves class to primitive token-type (integer or real).
*/
unsigned resolveTypeClass (const char *identifier,  unsigned class);

/* Resolves operation between two exprTypes. Returns resulting exprType.
 * 1. If `operator` involves divison, raises error if `b` is primitive and zero.
 * 2. Produces a warning if operands are mismatching primitive types.
 * 3. Promotes result if operands are mismatched.
*/

/* Returns resulting exprType of an operation between two exprTypes. 
 * 1. If operator involves division, throw div-zero-error if 'b' is zero.
 * 2. If operands are both primitives but mismatching, throw warning.
 * 3. If any operand has no constant value, then result is just the type.
 * Results are type-promoted in case of (2). */ 
exprType resolveOperation (unsigned operator, exprType a, exprType b);



#endif 