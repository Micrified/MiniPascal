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
*                             Identifier Prototypes                            *
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
*                            Expression Prototypes                             *
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
*                            Assignment Prototypes                             *
********************************************************************************
*/

/* Resolves assignment of expression to variable.
 * 1. If variable token-type is not primitive, an error is thrown.
 * 2. If expression token-type is not primitive, an error is thrown.
 * 3. Type promotion or truncation occurs in case of mismatching primitives.
 */ 
void resolveAssignment (varType var, exprType expr);

/*
********************************************************************************
*                      Function/Procedure Prototypes                           *
********************************************************************************
*/

/* Installs a function IdEntry into the symbol table.
 * 1. If function identifier in use, an error is thrown.
 * 2. If function has non-primitive return type, an error is thrown.
 * Function is installed with FUNCTION class variant of token-type.
*/
void installFunction (unsigned id, unsigned tt);

/* Installs variables in varList as arguments of function 'id'.
 * 1. Verifies 'id' exists, and has correct token-class.
 * 2. Install local variable with function name in new scope.
 * 3. Verifies variables in varlist have primitive token-types.
 * 4. Verifies variables in varlist have unique names.
 * Frees varList when finished. Scope must be incremented prior 
 * to using this function.
*/
void installFunctionArgs (unsigned id, varListType varList);

/* Verifies that expressions supplied to function identified by 'id'
 * match the parameter requirements. Does nothing if 'id' not a function.
 * to be a function when invoking this function.
 * 1) Verifies expression list count matches argument count.
 * 2) Verifies expression token-types match argument token-types.
*/
void verifyFunctionArgs (unsigned id, exprListType exprList);

/*
********************************************************************************
*                           Declaration Prototypes                             *
********************************************************************************
*/

/* Installs all given variables into the symbol table, then frees the list */
void installVarList (varListType varList);

#endif 