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

/* Displays warning if id of type-class tc is not initialized.
  * Only checks current table scope. If id doesn't exist in scope, no warning
  * is displayed.
*/ 
void isInitialized (unsigned id, unsigned tc);

/* Returns the token-type for an identifier. Must exist in symbol table. */
unsigned getIdTokenType (unsigned id, unsigned tc);

/*
********************************************************************************
*                          Variable-Expression Prototypes                      *
********************************************************************************
*/

/* Throws error if expression-variable type isn't of given token-class and token-type */
void requireExprVarType (unsigned tc, unsigned tt, varType var);

/* Returns resulting exprVarType of an operation between two exprVarTypes. 
 * 1. If operator involves division, throw div-zero-error if 'b' is zero.
 * 2. If any operand has no constant value, then result is just the token-type.
 * Results are type-promoted to reals if operands mismatch.  */ 
varType resolveArithmeticOperation (unsigned operator, varType a, varType b);

/* Returns resulting exprVarType for a boolean operation between two exprVarTypes.
 * 1. If any operand is undefined, an error is thrown.
 * 2. If any operand has no constant value, then result is just type MP_INTEGER.
 * Results of comparisons are always MP_INTEGER where defined.
*/
varType resolveBooleanOperation (unsigned operator, varType a, varType b);

/* Throws a warning if token-type of variable-expression isn't integer */
void verifyGuardExprVar (varType var);

/*
********************************************************************************
*                             Variable Prototypes                              *
********************************************************************************
*/

/* Extracts an IdEntry from the symbol table and initializes a varType instance.
 * Requires the IdEntry to already exist.
*/
varType initVarTypeFromId (unsigned id, unsigned tc);

/* Resolves assignment of expression-variable to variable.
 * 1. If variable token-class is not scalar, an error is thrown.
 * 2. If expression-variable token-type is undefined, an error is thrown.
 * 3. Type promotion or truncation occurs in case of mismatching primitives.
 * 4. Sets the reference flag (rf) to true.
 */ 
void verifyAssignment (varType var, varType exprVar);

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

/* Verifies that expression-variable supplied to routine identified by 'id'
 * match the parameter requirements. Requires routine IdEntry exist!
 * 1) Verifies expression-variable list count matches argument count.
 * 2) Verifies expression-variable token-types match argument token-types.
*/
void verifyRoutineArgs (unsigned id, varListType exprVarList);

/* Verifies that a function routine has an initialized return variable.
 * This function must be invoked before dropping the table scope level.
 */
void verifyFunctionReturnValue (unsigned id);

#endif