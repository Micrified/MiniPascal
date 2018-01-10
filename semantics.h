#if !defined(SYMANTICS_H)
#define SYMANTICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "mpascal.tab.h"
#include "mptypes.h"
#include "symtab.h"

/*
********************************************************************************
*                             Identifier Prototypes                            *
********************************************************************************
*/

/* Returns zero and throws error if id of type-class tc isn't in symbol table. */
unsigned existsId (unsigned id, unsigned tc);

/* Returns nonzero if the given id of type-class tc has been initialized. */
unsigned isInitialized (unsigned id, unsigned tc);

/* Returns the token-type for an identifier. Must exist in symbol table. */
unsigned getIdTokenType (unsigned id, unsigned tc);

/*
********************************************************************************
*                             Expression Prototypes                            *
********************************************************************************
*/

/* Throws error if expression type isn't of given token-type */
void requireExprType (unsigned tt, exprType expr);

/* Returns resulting exprType of an operation between two exprTypes. 
 * 1. If operator involves division, throw div-zero-error if 'b' is zero.
 * 2. If any operand has no constant value, then result is just the token-type.
 * Results are type-promoted to reals if operands mismatch.  */ 
exprType resolveArithmeticOperation (unsigned operator, exprType a, exprType b);

/* Returns resulting exprType for a boolean operation between two exprTypes.
 * 1. If any operand is undefined, an error is thrown.
 * 2. If any operand has no constant value, then result is just type MP_INTEGER.
 * Results of comparisons are always MP_INTEGER where defined.
*/
exprType resolveBooleanOperation (unsigned operator, exprType a, exprType b);


/* Throws an warning if the token-type of the expression isn't an integer */
void verifyGuardExpr (exprType expr);

/*
********************************************************************************
*                             Variable Prototypes                              *
********************************************************************************
*/

/* Extracts an IdEntry from the symbol table and initializes a varType instance.
 * Requires the IdEntry to already exist.
*/
varType initVarTypeFromId (unsigned id, unsigned tc);

/* Resolves assignment of expression to variable.
 * 1. If variable token-class is not scalar, an error is thrown.
 * 2. If expression token-type is undefined, an error is thrown.
 * 3. Type promotion or truncation occurs in case of mismatching primitives.
 * 4. Sets the reference flag (rf) to true.
 */ 
void verifyAssignment (varType var, exprType expr);

/* Maps a descriptor type to a list of varTypes. Returns varListType instance */
varListType mapDescToVarList (descType desc, varListType varList);

/* Installs a list of varTypes into the symbol table. 
 * 1. Verifies no variable already exists in current scope
 */
void installVarList (varListType varList);

/*
********************************************************************************
*                              Routine Prototypes                            *
********************************************************************************
*/

/* Installs a routine in the symbol table under 'id'.
 * 1) Verify id is unused (no entry in symbol table).
 * Procedures are identifier with an UNDEFINED token-type.
 * Returns nonzero if successful.
*/
unsigned installRoutine (unsigned id, unsigned tt);

/* Installs all routine arguments in the symbol table under 'id'. */
void installRoutineArgs (unsigned id, varListType varList);

/* Verifies that expressions supplied to function or procedure identified by 'id'
 * match the parameter requirements. Does nothing if 'id' not a routine.
 * 1) Verifies expression list count matches argument count.
 * 2) Verifies expression token-types match argument token-types.
*/
void verifyRoutineArgs (unsigned id, exprListType exprList);

/* Verifies that a function routine has an initialized return variable.
 * This function must be invoked before dropping the table scope level.
 */
void verifyFunctionReturnValue (unsigned id);

#endif