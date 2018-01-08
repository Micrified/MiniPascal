#include "mptypes.h"

/*
********************************************************************************
*                              Functions: Utility                              *
********************************************************************************
*/

/* Returns a string associated with the given token-type */
const char *tokenTypeName (unsigned tt) {
    switch (tt) {
        case TT_UNDEFINED:          return "Undefined";
        case TT_INTEGER:            return "Integer";
        case TT_REAL:               return "Real";
        case TT_ARRAY_INTEGER:      return "Integer[]";
        case TT_ARRAY_REAL:         return "Real[]";
        case TT_FUNCTION_INTEGER:   return "Integer()";
        case TT_FUNCTION_REAL:      return "Real()";
    }
    return "(Unknown Token-Type)";
}

/* Returns a string associated with the given token-class */
const char *tokenClassName (unsigned tc) {
    switch (tc) {
        case TC_ARRAY:              return "Array";
        case TC_FUNCTION:           return "Function";
    }
    return "(Unknown Token-Class)";
}

/*
********************************************************************************
*                               Functions: exprType                            *
********************************************************************************
*/

/* Initializes a new exprType with given token-tyke (tt) and value-index (vi) */
exprType initExprType (unsigned tt, unsigned vi) {
    return (exprType){.tt = tt, .vi = vi};
}

/*
********************************************************************************
*                             Functions: exprListType                          *
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
*                               Functions: varType                             *
********************************************************************************
*/

/* Initializes a new varType with the given token-type (tt) and identifier-index (id) */
varType initVarType (unsigned tt, unsigned id) {
    return (varType){.tt = tt, .id = id};
}

/*
********************************************************************************
*                             Functions: varListType                           *
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

/* Maps the given token-type to the list of varTypes in the varList list. */
varListType mapTokenTypeToVarList (unsigned tt, varListType varList) {
    for (int i = 0; i < varList.length; i++) {
        varList.list[i].tt = tt;
    }
    return varList;
}

/* Appends the first varListType list to second varListType */
varListType appendVarList (varListType suffix, varListType prefix) {

    // Compute new length of prefix.
    prefix.length += suffix.length;

    // Reallocate prefix list.
    if ((prefix.list = realloc(prefix.list, prefix.length)) == NULL) {
        fprintf(stderr, "Error: appendVarList: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }

    // Append copies of varTypes to prefix list.
    for (int i = prefix.length - suffix.length; i < prefix.length; i++) {
        prefix.list[i] = suffix.list[i];
    }

    // Free suffix list.
    free(suffix.list);

    return prefix;
}