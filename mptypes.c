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
    return "(?)";
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

    if ((exprList.list = realloc(exprList.list, exprList.length + 1)) == NULL) {
        fprintf(stderr, "Error: insertExprList: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }

    exprList.list[exprList.length++] = expr;

    return exprList;
}