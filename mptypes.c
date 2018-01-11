#include "mptypes.h"

/*
********************************************************************************
*                              Conversion Functions                            *
********************************************************************************
*/

/* Returns a string associated with the given token-type */
const char *tokenTypeName (unsigned tt) {
    switch (tt) {
        case UNDEFINED:             return "Undefined";
        case TT_INTEGER:            return "Integer";
        case TT_REAL:               return "Real";
    }
    return "(Unknown Token-Type)";
}

/* Returns a string associated with the given token-class */
const char *tokenClassName (unsigned tc) {
    switch (tc) {
        case UNDEFINED:             return "Undefined";
        case TC_SCALAR:             return "Scalar";
        case TC_VECTOR:             return "Vector";
        case TC_ROUTINE:            return "Routine";
    }
    return "(Unknown Token-Class)";
}
/*
********************************************************************************
*                              Expression Functions                            *
********************************************************************************
*/

/* Initializes a new exprType with given token-class (tc), token-type (tt),
 * and value-index (vi).
*/
exprType initExprType (unsigned tc, unsigned tt, unsigned vi) {
    return (exprType){.tc = tc, .tt = tt, .vi = vi};
}

/*
********************************************************************************
*                            Expression-List Functions                         *
********************************************************************************
*/

/* Initializes a new exprListType */
exprListType initExprListType (void) {
    return (exprListType){.length = 0, .list = NULL};
}

/* Frees allocated memory in an exprListType */
void freeExprList(exprListType exprList) {
    free(exprList.list);
}

/* Allocates a copy of the given exprType and places it in returned exprList list. */
exprListType insertExprList (exprType expr, exprListType exprList) {
    
    if ((exprList.list = realloc(exprList.list, (exprList.length + 1) * sizeof(exprType))) == NULL) {
        fprintf(stderr, "Error: insertExprList: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }

    exprList.list[exprList.length++] = expr;

    return exprList;
}

/*
********************************************************************************
*                               Variable Functions                             *
********************************************************************************
*/

/* Initializes a new varType with the given token-class (tc), token-type (tt) and 
 *identifier-index (id).
*/
varType initVarType (unsigned tc, unsigned tt, unsigned id) {
    return (varType){.tc = tc, .tt = tt, .id = id};
}

/*
********************************************************************************
*                            Variable-List Functions                           *
********************************************************************************
*/

/* Initializes a new varListType */
varListType initVarListType (void) {
    return (varListType){.length = 0, .list = NULL};
}

/* Frees allocated memory in a varListType */
void freeVarList(varListType varList) {
    free(varList.list);
}

/* Allocates a copy of the given varType and places it in returned varList list. */
varListType insertVarType (varType var, varListType varList) {
    
    if ((varList.list = realloc(varList.list, (varList.length + 1) * sizeof(varType))) == NULL) {
        fprintf(stderr, "Error: insertVarType: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }

    varList.list[varList.length++] = var;
    
    return varList;
}

/* Appends the first varListType list to second varListType */
varListType appendVarList (varListType suffix, varListType prefix) {

    // Compute new length of prefix.
    prefix.length += suffix.length;

    // Reallocate prefix list.
    if ((prefix.list = realloc(prefix.list, prefix.length * sizeof(varType))) == NULL) {
        fprintf(stderr, "Error: appendVarList: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }

    // Append copies of varTypes to prefix list.
    for (int i = 0; i < suffix.length; i++) {
        prefix.list[prefix.length - suffix.length + i] = suffix.list[i];
    }

    // Free suffix list.
    free(suffix.list);

    return prefix;   
}
