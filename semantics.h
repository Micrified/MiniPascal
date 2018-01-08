#if !defined(SEMANTICS_H)
#define SEMANTICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "mpascal.tab.h"
#include "mptypes.h"
#include "symtab.h"

/*
********************************************************************************
*                              General Prototypes                              *
********************************************************************************
*/


/*
********************************************************************************
*                               idType Prototypes                              *
********************************************************************************
*/

/* Returns token-type of identifier. If no entry, one is made with type `tt`. */
unsigned getTypeElseInstall (const char *identifier, unsigned tt);

/* Returns token-type of identifier. If no entry, an error is thrown */
unsigned getTypeElseError (const char *identifier);

/* Throws an error if the identifier entry is not of the expected class */
void requireIdClass (const char *identifier, unsigned class);

/*
********************************************************************************
*                              exprType Prototypes                             *
********************************************************************************
*/

/* Throws an error if the exprType token-type doesn't match type `tt` */
void requireExprType(unsigned tt, exprType expr);

/* Throws an error if the exprType token-type isn't of TT_INTEGER for guards */
void verifyGuardExpr (exprType expr);

/* Returns primitive token-type for expected type the given class.
 * 1. Throws undefined-type error if no IdEntry exists for given identifier.
 * 2. Throws unexpected-type error if IdEntry doesn't match expected class.
 * Resolves class to primitive token-type (integer or real).
*/
unsigned resolveTypeClass (const char *identifier,  unsigned class);

/* Returns resulting exprType of an arithmetic operation between two exprTypes. 
 * 1. If operator involves division, throw div-zero-error if 'b' is zero.
 * 2. If operands are both primitives but mismatching, throw warning.
 * 3. If any operand has no constant value, then result is just the type.
 * Results are type-promoted in case of (2). */ 
exprType resolveArithmeticOperation (unsigned operator, exprType a, exprType b);

/* Returns resulting exprType for a boolean operation between two exprTypes.
 * 1. If any operand is undefined, an error is thrown.
 * 2. If operators are both primitives but mismatching, throw warning.
 * 3. If any operand has no constant value, then result is just type MP_INTEGER.
 * Results of comparisons are always MP_INTEGER where defined.
*/
exprType resolveBooleanOperation (unsigned operator, exprType a, exprType b);

/*
********************************************************************************
*                               varType Prototypes                             *
********************************************************************************
*/

/* Resolves an assignment of an exprType to an identifier.
 * If the varType identifier has token-type undefined, an error is thrown.
 * If the exprType token-type doesn't match the identifier's token-type,
 *  an error is thrown.
 */
void resolveAssignment (varType var, exprType expr);

/*
********************************************************************************
*                            varListType Prototypes                            *
********************************************************************************
*/

/* Installs a function IdEntry into the symbol table without arguments.
 * 1. 'id' must not exist.
 * 2. 'tt' must be of token-class TC_FUNCTION.
*/
void installFunction (unsigned id, unsigned tt);

/* Installs all arguments in varList for the entry associated with 'id'.
 * 1. Verifies 'id' exists and has token-class TC_FUNCTION.
 * 2. Verifies arguments have proper token-types.
 * 3. Verifies arguments are not already defined.
 * 4. Frees varList when done. 
 * Note: Must increment scope level prior to this function.
*/
void installFunctionArgs (unsigned id, varListType varList);

#endif 