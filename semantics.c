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
    return (tc & tc);
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
*                              idType Functions                                *
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
*                             exprType Functions                               *
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
*                                varType Functions                             *
********************************************************************************
*/

/* Resolves an assignment of an exprType to an identifier.
 * If the varType identifier has non-primitive token-type, an error is thrown.
 * If the exprType token-type doesn't match the identifier's token-type,
 *  an error is thrown.
 */
void resolveAssignment (varType var, exprType expr) {

    // (*). Verify variable token-type is assignable.
    if (!isPrimitiveOperand(var.tt)) {
        printError("\"%s\" of type \"%s\" is not assignable!",
        identifierAtIndex(var.id), tokenTypeName(var.tt));
    }

    // (*). Verify that expression token-type is primitive.
    if (!isPrimitiveOperand(expr.tt)) {
        printError("\"%s\" may not be assigned to \"%s\"!", 
        tokenTypeName(expr.tt), tokenTypeName(var.tt));
    }

    // (*). Reduce variable token-type.
    var.tt = reduceValidOperandType(var.tt);

    // (*). Verify exprType token-type matches.
    if (var.tt != expr.tt) {
        printWarning("Assigned value will be %s!", 
        (var.tt == TT_INTEGER) ? "truncated" : "promoted");
    }
}

/*
********************************************************************************
*                             varListType Functions                            *
********************************************************************************
*/

/* Installs a function IdEntry into the symbol table without arguments.
 * 1. 'id' must not exist.
 * 2. 'tt' must be of token-class TC_FUNCTION.
*/
void installFunction (unsigned id, unsigned tt) {
    IdEntry *entry;

    // (*). Verify function identifier is unused. Else throw error.
    if ((entry = tableContains(identifierAtIndex(id))) != NULL) {
        printError("\"%s\" is already defined as \"%s\"!", identifierAtIndex(id), 
        tokenTypeName(entry->tt));
        return;
    }

    // (*). Verify function return token-type is  a primitive. Else throw error.
    if (!isPrimitiveOperand(tt)) {
        printError("\"%s\" is not a valid function return type!", tokenTypeName(tt));
        return;
    }

    // (*). Install function in symbol table with function-class of given token-type.
    unsigned ft = (tt == TT_INTEGER) ? TT_FUNCTION_INTEGER : TT_FUNCTION_REAL;
    entry = installEntry(newIDEntry(id, ft));
}

/* Installs all arguments in varList for the entry associated with 'id'.
 * 1. Verifies 'id' exists and has token-class TC_FUNCTION.
 * 2. Verifies arguments have proper token-types.
 * 3. Verifies arguments are not already defined.
 * 4. Frees varList when done. 
 * Note: Must increment scope level prior to this function.
*/
void installFunctionArgs (unsigned id, varListType varList) {
    IdEntry *entry, **argv;

    // (*). Verify entry exists, and has function token-type.
    if ((entry = tableContains(identifierAtIndex(id))) == NULL || !isTokenClass(TC_FUNCTION, entry->tt)) {
        fprintf(stderr, "Error: installFunctionArgs: \"%s\" doesn't exist or doesn't have token-class \"%s\"!\n",
        identifierAtIndex(id), tokenClassName(TC_FUNCTION));
        exit(EXIT_FAILURE);
    }

    // (*). Install local variable with same name as function but with primitive type.
    installEntry(newIDEntry(id, reduceValidOperandType(entry->tt)));

    // (*). Verify argument list contains valid token-types.
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

    // (*). Install arguments (should be no existing entries).
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
    entry->argv = argv;

    // (*). Free variable list.
    freeVarList(varList);

    printf("DEBUG: Verify tables match expected state!\n");
    printStringTable();
    printSymbolTables();
}

