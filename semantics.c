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
        case MP_ADDOP: return a + b;
        case MP_DIVOP: return a / b;
        case MP_MULOP: return a * b;
        case MP_MODOP: return (double)((int)a % (int)b);
    }
    fprintf(stderr, "Error: performOperation: Unknown operator %u!\n", operator);
    exit(EXIT_FAILURE);
}

/* Returns nonzero if the given token-type may be used in an arithmetic operation */
static int isValidOperand (unsigned tt) {
    return (tt > 0 && tt <= TT_FUNCTION_REAL);
}

/* Returns nonzero if the given token-type is a primitive */
static int isPrimitiveOperand (unsigned tt) {
    return (tt > 0 && tt < TC_ARRAY);
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
*                                 Functions                                    *
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

/* Returns primitive token-type for expected type the given class.
 * 1. Throws undefined-type error if no IdEntry exists for given identifier.
 * 2. Throws unexpected-type error if IdEntry doesn't match expected class.
 * Resolves class to primitive token-type (integer or real).
*/
unsigned resolveTypeClass (const char *identifier,  unsigned class) {
    IdEntry *entry;

    // Verify: IdEntry exists. 
    if ((entry = tableContains(identifier)) == NULL) {
        fprintf(stderr, "Error: %s is undefined!\n", identifier);
        exit(EXIT_FAILURE);
    }

    // Verify: Token-Type is of expected class.
    if ((entry->tt & class) == 0) {
        fprintf(stderr, "Error: %s is not of class %u!\n", identifier, mask);
        exit(EXIT_FAILURE);
    }

    // Resolve class to primitive: 1 + (tt % 2) => (integer = 1, real = 2).
    return reduceValidOperandType(entry->tt);
}

/* Returns resulting exprType of an operation between two exprTypes. 
 * 1. If operands are both primitives but mismatching, throw warning.
 * 2. If operator involves division, throw div-zero-error if 'b' is zero.
 * 3. If any operand has no constant value, then result is just the type.
 * Results are type-promoted in case of (2). */ 
exprType resolveOperation (unsigned operator, exprType a, exprType b) {
    double *vp;

    // (*). Verify operands are valid.
    if (!isValidOperand(a.tt) || !isValidOperand(b.tt)) {
        fprintf(stderr, "Error: Illegal operation between %u and %u!\n", a.tt, b.tt);
        exit(EXIT_FAILURE);
    }

    // (*). Reduce types to primitives if necessary.
    a.tt = reduceValidOperandType(a.tt);
    b.tt = reduceValidOperandType(b.tt);

    // (1). Throw warning if primitive types mismatch.
    if (isPrimitiveOperand(a.tt) && isPrimitiveOperand(b.tt) && a.tt != b.tt) {
        fprintf(stderr, "Warning: Mismatching types in operation (%u v %u)!\n", a.tt, b.tt);
    }

    // (2). Check for division by zero. 
    if ((operator == MP_DIVOP || operator == MP_MODOP) && 
        (vp = numberAtIndex(b.vi)) != NULL && (*vp == 0.0)) {
        fprintf(stderr, "Error: Division-by-zero!\n");
        exit(EXIT_FAILURE);
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