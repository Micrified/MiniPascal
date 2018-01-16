#include "semantics.h"

#define MAX(a,b)        ((a) > (b) ? (a) : (b))

/*
********************************************************************************
*                         Internal Expression Functions                        *
********************************************************************************
*/

/* Performs an operation based on the given lexer token */
static double performOperation (unsigned operator, double a, double b) {
    switch (operator) {

        // Arithmetic Operators.
        case MP_ADDOP: return a + b;
        case MP_DIVOP: return a / b;
        case MP_MULOP: return a * b;
        case MP_MODOP: return (double)((int)a % (int)b);

        // Boolean Operators.
        case MP_RELOP_LT: return (a < b);
        case MP_RELOP_LE: return (a <= b);
        case MP_RELOP_EQ: return (a == b);
        case MP_RELOP_GE: return (a >= b);
        case MP_RELOP_GT: return (a > b);
        case MP_RELOP_NE: return (a != b);
    }
    fprintf(stderr, "Error: performOperation: Unknown operator %d\n", operator);
    exit(EXIT_FAILURE);
}

/*
********************************************************************************
*                             Identifier Functions                             *
********************************************************************************
*/

/* Returns zero and throws error if id of type-class tc isn't in symbol table. */
unsigned existsId (unsigned id, unsigned tc) {
    IdEntry *entry;

    if ((entry = containsIdEntry(id, tc, SYMTAB_SCOPE_ALL)) == NULL) {
        printError("Expected identifier \"%s\" of class \"%s\", but none exists!",
            identifierAtIndex(id),
            tokenClassName(tc));
        return 0;
    }
    return 1;
}

/* Displays warning if id of type-class tc is not initialized.
  * Only checks current table scope. If id doesn't exist in scope, no warning
  * is displayed.
*/ 
void isInitialized (unsigned id, unsigned tc) {
    IdEntry *entry;
    if ((entry = containsIdEntry(id, tc, currentTableScope())) != NULL && entry->rf == 0) {
        printWarning("\"%s\" of class \"%s\" exists but is uninitialized!", 
            identifierAtIndex(entry->id),
            tokenClassName(entry->tc));
    }
}

/* Returns the token-type for an identifier. Must exist in symbol table. */
unsigned getIdTokenType (unsigned id, unsigned tc) {
    IdEntry *entry = containsIdEntry(id, tc, SYMTAB_SCOPE_ALL);
    return entry->tt;
}

/*
********************************************************************************
*                             Expression Functions                             *
********************************************************************************
*/

/* Throws error if expression-variable type isn't of given token-class and token-type */
void requireExprVarType (unsigned tc, unsigned tt, varType var) {
    if (var.tc != tc) {
        printError("Expected class \"%s\" but got class \"%s\" instead!",
            tokenClassName(tc), tokenClassName(var.tc));
    }
    if (var.tt != tt) {
        printError("Expected type \"%s\" but got type \"%s\" instead!",
            tokenTypeName(tt), tokenTypeName(var.tt));
    }
}

/* Returns resulting exprVarType of an operation between two exprVarTypes. 
 * 1. If operator involves division, throw div-zero-error if 'b' is zero.
 * 2. If any operand has no constant value, then result is just the token-type.
 * Results are type-promoted to reals if operands mismatch.  */ 
varType resolveArithmeticOperation (unsigned operator, varType a, varType b) {
    double *vp;

    // (*). Verify operands have correct token-class.
    if (a.tc != TC_SCALAR || b.tc != TC_SCALAR) {
        printError("Arithmetic operation is undefined for non-scalar operands!");
        return initExprVarType(UNDEFINED, UNDEFINED, NIL);
    }

    // (*). Verify operands have correct token-type.
    if (a.tt == UNDEFINED || b.tt == UNDEFINED) {
        printError("Arithmetic operation is undefined for operands of type: \"%s\"!\n", 
        tokenTypeName(UNDEFINED));
        return initExprVarType(TC_SCALAR, UNDEFINED, NIL);
    }

    // (1). Check for division by zero. 
    if ((operator == MP_DIVOP || operator == MP_MODOP) && 
        (vp = numberAtIndex(b.vi)) != NULL && (*vp == 0.0)) {
        printError("Division by zero!");
        return initExprVarType(TC_SCALAR, UNDEFINED, NIL);
    }

    // (2). Determine resulting constant value.
    double *avp, *bvp, newValueIndex;
    if ((avp = numberAtIndex(a.vi)) == NULL || (bvp = numberAtIndex(b.vi)) == NULL) {
        newValueIndex = NIL;
    } else {
        newValueIndex = installNumber(performOperation(operator, *avp, *bvp));
    }

    // (*). Return new exprVarType. Type promote resulting type if mismatched.
    return initExprVarType(TC_SCALAR, MAX(a.tt, b.tt), newValueIndex);
}

/* Returns resulting exprVarType for a boolean operation between two exprVarTypes.
 * 1. If any operand is undefined, an error is thrown.
 * 2. If any operand has no constant value, then result is just type MP_INTEGER.
 * Results of comparisons are always MP_INTEGER where defined.
*/
varType resolveBooleanOperation (unsigned operator, varType a, varType b) {

    // (*). Verify operands have correct token-class.
    if (a.tc != TC_SCALAR || b.tc != TC_SCALAR) {
        printError("Boolean operation is undefined for non-scalar operands!");
        return initExprVarType(UNDEFINED, UNDEFINED, NIL);
    }

    // (*). Verify operands have correct token-type.
    if (a.tt == UNDEFINED || b.tt == UNDEFINED) {
        printError("Boolean operation is undefined for operands of type: \"%s\"!\n", 
        tokenTypeName(UNDEFINED));
        return initExprVarType(TC_SCALAR, UNDEFINED, NIL);
    }

    // (2). Determine resulting constant value.
    double *avp, *bvp, newValueIndex;
    if ((avp = numberAtIndex(a.vi)) == NULL || (bvp = numberAtIndex(b.vi)) == NULL) {
        newValueIndex = NIL;
    } else {
        newValueIndex = installNumber(performOperation(operator, *avp, *bvp));
    }

    // (*). Return new exprVarType.
    return initExprVarType(TC_SCALAR, TT_INTEGER, newValueIndex);
}

/* Throws a warning if token-type of variable-expression isn't integer */
void verifyGuardExprVar (varType var) {
    if (var.tc != TC_SCALAR || var.tt == UNDEFINED) {
        printError("Guard expression must a \"%s\" of type \"%s\". Got \"%s\" of type \"%s\"!",
            tokenClassName(TC_SCALAR), tokenTypeName(TT_INTEGER), tokenClassName(var.tc),
            tokenTypeName(var.tt));
        return;
    }
    if (var.tt != TT_INTEGER) {
        printWarning("Guard expression will be truncated!");
    }
}

/*
********************************************************************************
*                             Variable Functions                               *
********************************************************************************
*/

/* Extracts an IdEntry from the symbol table and initializes a varType instance.
 * Requires the IdEntry to already exist.
*/
varType initVarTypeFromId (unsigned id, unsigned tc) {
    IdEntry *entry;

    // (*). Extract IdEntry, verify exists.
    if ((entry = containsIdEntry(id, tc, SYMTAB_SCOPE_ALL)) == NULL) {
        fprintf(stderr, "Error: initVarTypeFromId: Null entry for \"%s\"!\n", identifierAtIndex(id));
        exit(EXIT_FAILURE);
    }

    return initVarType(entry->tc, entry->tt, id);
}

/* Resolves assignment of expression-variable to variable.
 * 1. If variable token-class is not scalar, an error is thrown.
 * 2. If expression-variable token-type is undefined, an error is thrown.
 * 3. Type promotion or truncation occurs in case of mismatching primitives.
 * 4. Sets the reference flag (rf) to true.
 */ 
void verifyAssignment (varType var, varType exprVar) {

    // (1). Verify variable token-class is assignable.
    if (var.tc != TC_SCALAR && var.tc != TC_VECTOR) {
        printError("\"%s\" of class \"%s\" is not assignable!",
        identifierAtIndex(var.id), tokenClassName(var.tc));
        return;
    }

    // (2). Verify that expression-variable token-type is primitive.
    if (exprVar.tt == UNDEFINED) {
        printError("\"%s\" may not be assigned to \"%s\" \"%s\"!", 
        tokenTypeName(exprVar.tt), tokenClassName(var.tc), tokenTypeName(var.tt));
        return;
    }

    // (*). Extract IdEntry of id in var (Expects IdEntry to exist).
    IdEntry *entry = containsIdEntry(var.id, var.tc, SYMTAB_SCOPE_ALL);

    // (3). Verify exprVarType token-type matches.
    if (entry->tt < exprVar.tt) {
        printWarning("Value assigned to \"%s\" \"%s\" will be truncated!",
            tokenTypeName(entry->tt),
            identifierAtIndex(var.id)
        );
    }

    // (4). Set referenced flag to true.
    entry->rf = 1;
}

/* Maps a descriptor type to a list of varTypes. Returns varListType instance */
varListType mapDescToVarList (descType desc, varListType varList) {
    for (int i = 0; i < varList.length; i++) {
        varList.list[i].tc = desc.tc;
        varList.list[i].tt = desc.tt;
    }
    return varList;
}

/* Installs a list of varTypes into the symbol table. 
 * 1. Verifies no variable already exists in current scope
 */
void installVarList (varListType varList) {
    IdEntry *entry;

    for (int i = 0; i < varList.length; i++) {
        varType var = varList.list[i];
        if ((entry = containsIdEntry(var.id, var.tc, currentTableScope())) != NULL) {
            printError("Redeclaration of \"%s\" \"%s\" in current scope!",
                tokenClassName(var.tc), identifierAtIndex(var.id));
        } else {
            installIdEntry(var.id, var.tc, var.tt);
        }
    }
}

/*
********************************************************************************
*                              Routine Functions                               *
********************************************************************************
*/

/* Installs a routine in the symbol table under 'id'.
 * 1) Verify id is unused (no entry in symbol table).
 * Procedures are identifier with an UNDEFINED token-type.
 * Returns nonzero if successful.
*/
unsigned installRoutine (unsigned id, unsigned tt) {
    IdEntry *entry;

    // (1). Verify id is unused.
    if ((entry = containsIdEntry(id, TC_ROUTINE, SYMTAB_SCOPE_ALL)) != NULL) {
        printError("Illegal redefinition of \"%s\" \"%s\"!",
            tokenClassName(entry->tc), identifierAtIndex(id));
        return 0;
    }

    // (*). Install routine in symbol table.
    installIdEntry(id, TC_ROUTINE, tt);
    return 1;
}

/* Installs all routine arguments in the symbol table under 'id'. */
void installRoutineArgs (unsigned id, varListType varList) {
    IdEntry *arg, **argv;

    // (*). Extract IdEntry.
    IdEntry *entry = containsIdEntry(id, TC_ROUTINE, SYMTAB_SCOPE_ALL);

    // (*). Install new local scalar with routine token-type if function.
    if (entry->tt != UNDEFINED) {
        installIdEntry(id, TC_SCALAR, entry->tt);
    }

    // (*). Verify argument-list token-class is scalar or vector.
    for (int i = 0; i < varList.length; i++) {
        varType var = varList.list[i];
        if (var.tc != TC_SCALAR && var.tc != TC_VECTOR) {
            printError("Parameter \"%s\" in routine \"%s\" has illegal type-class \"%s\"!",
            identifierAtIndex(var.id), identifierAtIndex(id), tokenClassName(var.tc));
            return;
        }
    }

    // (*). Allocate pointer array for arguments.
    if ((argv = malloc(varList.length * sizeof(IdEntry *))) == NULL) {
        fprintf(stderr, "Error: installRoutineArgs: Couldn't allocate pointer array!\n");
        exit(EXIT_FAILURE);
    }

    // (*). Install arguments (should be no existing entries).
    for (int i = 0; i < varList.length; i++) {
        varType var = varList.list[i];
        if (containsIdEntry(var.id, var.tc, currentTableScope())) {
            printError("Duplicate parameters (\"%s\" \"%s\") in function \"%s\"!",
            tokenClassName(var.tc), identifierAtIndex(var.id), identifierAtIndex(id));
            return;
        }
        arg = installIdEntry(var.id, var.tc, var.tt);
        arg->rf = 1;
        argv[i] = copyIdEntry(arg);
    }

    // (*). Assign argument vector and length to IdEntry entry.
    entry->data.argc = varList.length;
    entry->data.argv = (void **)argv;
}

/* Verifies that expression-variable supplied to routine identified by 'id'
 * match the parameter requirements. Requires routine IdEntry exist!
 * 1) Verifies expression-variable list count matches argument count.
 * 2) Verifies expression-variable token-types match argument token-types.
*/
void verifyRoutineArgs (unsigned id, varListType exprVarList) {
    IdEntry *entry;

    // (*). Extract IdEntry, verify exists.
    if ((entry = containsIdEntry(id, TC_ROUTINE, SYMTAB_SCOPE_ALL)) == NULL) {
        fprintf(stderr, "Error: verifyRoutineArgs: Null entry!\n");
        exit(EXIT_FAILURE);
    }

    // (1). Verify length matches.
    if (entry->data.argc != exprVarList.length) {
        printError("\"%s\" requires %d arguments, not %d!", 
        identifierAtIndex(id), entry->data.argc, exprVarList.length);
        return;
    }

    // (2). Verify token-class matches and warn for token-type tuncations.
    for (int i = 0; i < exprVarList.length; i++) {
        IdEntry *arg = (IdEntry *)entry->data.argv[i];
        varType exprVar = exprVarList.list[i];

        if (exprVar.tc != arg->tc) {
            printError("Parameter %d of routine \"%s\" expects type-class \"%s\" but got \"%s\"!",
                i + 1, identifierAtIndex(id), tokenClassName(arg->tc), tokenClassName(exprVar.tc));
            continue;
        }

        if (exprVar.tt > arg->tt) {
            printWarning("Argument %d of routine \"%s\" will be truncated!\n",
                i + 1, identifierAtIndex(id));
        }
    }
}

/* Verifies that a function routine has an initialized return variable.
 * This function must be invoked before dropping the table scope level.
 */
void verifyFunctionReturnValue (unsigned id) {
    IdEntry *entry;

    // (*). Extract entry.
    if ((entry = containsIdEntry(id, TC_SCALAR, currentTableScope())) == NULL) {
        fprintf(stderr, "Error: verifyFunctionReturnValue: Return variable not in function scope!\n");
        exit(EXIT_FAILURE);
    }

    // (*). Verify initialization.
    if (entry->rf == 0) {
        printError("Return value for function \"%s\" is uninitialized!", identifierAtIndex(id));
    }
}
