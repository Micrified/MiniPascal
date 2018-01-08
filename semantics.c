#include "semantics.h"

#define MAX(a,b)        ((a) > (b) ? (a) : (b))

/*
********************************************************************************
*                          Private Semantic Routines                           *
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
    printError("performOperation: Unknown operator %d\n", operator);
    exit(EXIT_FAILURE);
}

/* Returns nonzero if the given token-type may be used in an arithmetic operation */
static int isValidOperand (unsigned tt, unsigned *tp) {
    *tp = tt;
    return (tt > TT_UNDEFINED && tt <= TT_FUNCTION_REAL);
}

/* Returns nonzero if the given token-type is a primitive */
static int isPrimitiveOperand (unsigned tt) {
    return (tt > TT_UNDEFINED && tt < TC_ARRAY);
}

/* Returns nonzero if the given token-type is of the given token-class */
static int isTokenClass (unsigned tc, unsigned tt) {
    return (tt & tc);
}

/* Reduces a valid operand type to a primitive type */
static unsigned reduceValidOperandType (unsigned tt) {
    if (isPrimitiveOperand(tt)) {
        return tt;
    } else {
        return 1 + (tt % 2);
    }
}

/*
********************************************************************************
*                           Identifier Functions                               *
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

/* Returns token-type of identifier. If no entry, an error is thrown. */
unsigned getTypeElseError (const char *identifier) {
    IdEntry *entry;

    // Check if entry exists, otherwise throw error.
    if ((entry = tableContains(identifier)) == NULL) {
        printError("\"%s\" is undefined!", identifier);
        return TT_UNDEFINED;
    }

    return entry->tt;
}

/* Throws an error if the identifier entry is not of the expected class */
void requireIdClass (const char *identifier, unsigned class) {
    IdEntry *entry;

    // (*). Check if entry exists, otherwise throw error.
    if ((entry = tableContains(identifier)) == NULL) {
        printError("\"%s\" is undefined!");
    }

    // (*). Verify token-type is of expected class.
    if ((entry->tt & class) == 0) {
        printError("\"%s\" is not of expected type-class \"%s\"!", 
        tokenTypeName(entry->tt), 
        tokenClassName(class));
    }
}

/*
********************************************************************************
*                             Expression Functions                             *
********************************************************************************
*/

/* Throws an error if the exprType token-type doesn't match type `tt` */
void requireExprType(unsigned tt, exprType expr) {
    if (expr.tt != tt) {
        printError("Expected type \"%s\", but got type \"%s\" instead!", 
        tokenTypeName(tt), tokenTypeName(expr.tt));
    }
}

/* Throws an error if the exprType token-type isn't of TT_INTEGER for guards */
void verifyGuardExpr (exprType expr) {
    if (expr.tt != TT_INTEGER) {
        printError("Guard expression has type \"%s\". Only type \"%s\" allowed!", 
        tokenTypeName(expr.tt), tokenTypeName(TT_INTEGER));
    }
}

/* Returns primitive token-type for expected type the given class.
 * 1. Throws undefined-type error if no IdEntry exists for given identifier.
 * 2. Throws unexpected-type error if IdEntry doesn't match expected class.
 * Resolves class to primitive token-type (integer or real).
*/
unsigned resolveTypeClass (const char *identifier,  unsigned class) {
    IdEntry *entry;

    // Verify: IdEntry exists. 
    if ((entry = tableContains(identifier)) == NULL) {
        printError("\"%s\" is undefined!", identifier);
        return TT_UNDEFINED;
    }

    // Verify: Token-Type is of expected class.
    if ((entry->tt & class) == 0) {
        printError("\"%s\" is not of expected type-class \"%s\"!", identifier, tokenClassName(class));
        return TT_UNDEFINED;
    }

    // Resolve class to primitive: 1 + (tt % 2) => (integer = 1, real = 2).
    return reduceValidOperandType(entry->tt);
}

/* Returns resulting exprType of an operation between two exprTypes. 
 * 1. If operands are both primitives but mismatching, throw warning.
 * 2. If operator involves division, throw div-zero-error if 'b' is zero.
 * 3. If any operand has no constant value, then result is just the type.
 * Results are type-promoted in case of (2). */ 
exprType resolveArithmeticOperation (unsigned operator, exprType a, exprType b) {
    unsigned t;
    double *vp;

    // (*). Verify operands are valid.
    if (!isValidOperand(a.tt, &t) || !isValidOperand(b.tt, &t)) {
        printError("Arithmetic operation is undefined for type: \"%s\"!\n", tokenTypeName(t));
        return (exprType){.tt = TT_UNDEFINED, .vi = NIL};
    }

    // (*). Reduce types to primitives if necessary.
    a.tt = reduceValidOperandType(a.tt);
    b.tt = reduceValidOperandType(b.tt);

    // (1). Throw warning if primitive types mismatch.
    if (isPrimitiveOperand(a.tt) && isPrimitiveOperand(b.tt) && a.tt != b.tt) {
        printWarning("Mismatching types in arithmetic expression!");
    }

    // (2). Check for division by zero. 
    if ((operator == MP_DIVOP || operator == MP_MODOP) && 
        (vp = numberAtIndex(b.vi)) != NULL && (*vp == 0.0)) {
        printError("Division by zero!");
        return (exprType){.tt = TT_UNDEFINED, .vi = NIL};
    }

    // (3). Determine resulting constant value.
    double *avp, *bvp, newValueIndex;
    if ((avp = numberAtIndex(a.vi)) == NULL || (bvp = numberAtIndex(b.vi)) == NULL) {
        newValueIndex = NIL;
    } else {
        newValueIndex = installNumber(performOperation(operator, *avp, *bvp));
    }

    // (*). Return new exprType. Type promote resulting type if mismatched.
    return (exprType){.tt = MAX(a.tt, b.tt), .vi = newValueIndex};
}

/* Returns resulting exprType for a boolean operation between two exprTypes.
 * 1. If any operand is undefined, an error is thrown.
 * 2. If operators are both primitives but mismatching, throw warning.
 * 3. If any operand has no constant value, then result is just type MP_INTEGER.
 * Results of comparisons are always MP_INTEGER where defined.
*/
exprType resolveBooleanOperation (unsigned operator, exprType a, exprType b) {
    unsigned t;

    // (1). Verify operands are valid!
    if (!isValidOperand(a.tt, &t) || !isValidOperand(b.tt, &t)) {
        printError("Boolean operation is undefined for type: \"%s\"!\n", tokenTypeName(t));
        return (exprType){.tt = TT_UNDEFINED, .vi = NIL};
    }

    // (*). Reduce types to primitives if necessary.
    a.tt = reduceValidOperandType(a.tt);
    b.tt = reduceValidOperandType(b.tt);

    // (2). Throw warning if primitive types mismatch.
    if (isPrimitiveOperand(a.tt) && isPrimitiveOperand(b.tt) && a.tt != b.tt) {
        printWarning("Mismatching types in boolean expression!");
    }

    // (3). Determine resulting constant value.
    double *avp, *bvp, newValueIndex;
    if ((avp = numberAtIndex(a.vi)) == NULL || (bvp = numberAtIndex(b.vi)) == NULL) {
        newValueIndex = NIL;
    } else {
        newValueIndex = installNumber(performOperation(operator, *avp, *bvp));
    }

    return (exprType){.tt = TT_INTEGER, .vi = newValueIndex};
}

/*
********************************************************************************
*                             Assignment Functions                             *
********************************************************************************
*/

/* Resolves assignment of expression to variable.
 * 1. If variable token-type is not primitive, an error is thrown.
 * 2. If expression token-type is not primitive, an error is thrown.
 * 3. Type promotion or truncation occurs in case of mismatching primitives.
 */ 
void resolveAssignment (varType var, exprType expr) {

    // (1). Verify variable token-type is assignable.
    if (!isPrimitiveOperand(var.tt)) {
        printError("\"%s\" of type \"%s\" is not assignable!",
        identifierAtIndex(var.id), tokenTypeName(var.tt));
    }

    // (2). Verify that expression token-type is primitive.
    if (!isPrimitiveOperand(expr.tt)) {
        printError("\"%s\" may not be assigned to \"%s\"!", 
        tokenTypeName(expr.tt), tokenTypeName(var.tt));
    }

    // (*). Reduce variable token-type.
    var.tt = reduceValidOperandType(var.tt);

    // (3). Verify exprType token-type matches.
    if (var.tt != expr.tt) {
        printWarning("Assigned value will be %s!", 
        (var.tt == TT_INTEGER) ? "truncated" : "promoted");
    }
}

/*
********************************************************************************
*                        Function/Procedure Functions                          *
********************************************************************************
*/

/* Installs a function IdEntry into the symbol table.
 * 1. If function identifier in use, an error is thrown.
 * 2. If function has non-primitive return type, an error is thrown.
 * Function is installed with FUNCTION class variant of token-type.
*/
void installFunction (unsigned id, unsigned tt) {
    IdEntry *entry;

    // (1). Verify function identifier is unused. Else throw error.
    if ((entry = tableContains(identifierAtIndex(id))) != NULL) {
        printError("\"%s\" is already defined as \"%s\"!", identifierAtIndex(id), 
        tokenTypeName(entry->tt));
        return;
    }

    // (2). Verify function return token-type is  a primitive. Else throw error.
    if (!isPrimitiveOperand(tt)) {
        printError("\"%s\" is not a valid function return type!", tokenTypeName(tt));
        return;
    }

    // (*). Install function in symbol table with function-class of given token-type.
    unsigned ft = (tt == TT_INTEGER) ? TT_FUNCTION_INTEGER : TT_FUNCTION_REAL;
    entry = installEntry(newIDEntry(id, ft));
}

/* Installs variables in varList as arguments of function 'id'.
 * 1. Verifies 'id' exists, and has correct token-class.
 * 2. Install local variable with function name in new scope.
 * 3. Verifies variables in varlist have primitive token-types.
 * 4. Verifies variables in varlist have unique names.
 * Frees varList when finished. Scope must be incremented prior 
 * to using this function.
*/
void installFunctionArgs (unsigned id, varListType varList) {
    IdEntry *entry, **argv;

    // (1). Verify entry exists, and has token-class TC_FUNCTION.
    if ((entry = tableContains(identifierAtIndex(id))) == NULL || !isTokenClass(TC_FUNCTION, entry->tt)) {
        fprintf(stderr, "Error: installFunctionArgs: \"%s\" doesn't exist or doesn't have token-class \"%s\"!\n",
        identifierAtIndex(id), tokenClassName(TC_FUNCTION));
        exit(EXIT_FAILURE);
    }

    // (2). Install local variable with same name as function but with primitive type.
    installEntry(newIDEntry(id, reduceValidOperandType(entry->tt)));

    // (3). Verify argument list contains valid token-types.
    for (int i = 0; i < varList.length; i++) {
        printf("Checking no.%d (varType) {.id = %u, .tt = %s}\n", i, varList.list[i].id, tokenTypeName(varList.list[i].tt));
        if (!isPrimitiveOperand(varList.list[i].tt)) {
            printError("Parameter \"%s\" in function \"%s\" has illegal type \"%s\"!", 
            identifierAtIndex(varList.list[i].id),
            identifierAtIndex(id),
            tokenTypeName(varList.list[i].tt));
            return;
        }
    }

    // (*). Allocate pointer array for arguments.
    if ((argv = malloc(varList.length * sizeof(IdEntry *))) == NULL) {
        fprintf(stderr, "Error: installFunction: Couldn't allocate argv!\n");
        exit(EXIT_FAILURE);
    }

    // (4). Install arguments (should be no existing entries).
    for (int i = 0; i < varList.length; i++) {
        varType v = varList.list[i];

        // If entry exists, then duplicate argument name.
        if (tableContains(identifierAtIndex(v.id)) != NULL) {
            printError("Duplicate parameter names in function \"%s\"!", identifierAtIndex(id));
            return;
        }
        argv[i] = installEntry(newIDEntry(v.id, v.tt));
    }

    // (*). Assign argument vector and length to function entry.
    entry->argc = varList.length;
    entry->argv = (void **)argv;

    // (5). Free variable list.
    freeVarList(varList);

    printf("DEBUG: Verify tables match expected state!\n");
    printStringTable();
    printSymbolTables();
}

/* Verifies that expressions supplied to function identified by 'id'
 * match the parameter requirements. Does nothing if 'id' not a function.
 * to be a function when invoking this function.
 * 1) Verifies expression list count matches argument count.
 * 2) Verifies expression token-types match argument token-types.
*/
void verifyFunctionArgs (unsigned id, exprListType exprList) {
    IdEntry *entry;

    // (*). Verify argument given is a function. Return otherwise.
    if ((entry = tableContains(identifierAtIndex(id))) == NULL || !isTokenClass(TC_FUNCTION, entry->tt)) {
        return;
    }

    // (1). Verify expression list count matches argument count. 
    if (exprList.length != entry->argc) {
        printError("\"%s\" requires %d arguments, not %d!", identifierAtIndex(id), entry->argc, exprList.length);
        return;
    }

    // (2). Verify expression token-types match argument token-types.
    for (int i = 0; i < entry->argc; i++) {
        IdEntry *arg = (IdEntry *)entry->argv[i];
        exprType expr = exprList.list[i];

        if (arg->tt == expr.tt) {
            continue;
        }

        if (!isPrimitiveOperand(expr.tt)) {
            printError("Parameter %d of function \"%s\" expected \"%s\" but got \"%s\" instead!",
            i + 1, identifierAtIndex(id), tokenTypeName(arg->tt), tokenTypeName(exprList.list[i].tt));
            continue;
        }

        printWarning("Parameter %d of function \"%s\" will be %s!", 
        i + 1, identifierAtIndex(id), (expr.tt == TT_INTEGER) ? "promoted" : "truncated");
    }
}

