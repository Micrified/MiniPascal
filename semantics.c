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

    // (*). Verify identifier token-type is assignable.
    if (!isPrimitiveOperand(var.tt)) {
        printError("\"%s\" of type \"%s\" is not assignable!",
        identifierAtIndex(var.id), tokenTypeName(var.tt));
    }

    // (*). Verify exprType token-type matches.
    if (var.tt != expr.tt) {
        printError("\"%s\" of type \"%s\" may not be assigned a value of type \"%s\"!", 
        identifierAtIndex(var.id), tokenTypeName(var.tt), tokenTypeName(expr.tt));
    }
}
