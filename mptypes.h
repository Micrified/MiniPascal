#if !defined(MPTYPES_H)
#define MPTYPES_H

#include <stdio.h>
#include <stdlib.h>

/*
    ***************************************************************************
    *                   Types used in MiniPascal Compiler                     *
    * AUTHORS: Charles Randolph, Joe Jones.                                   *
    * SNUMBERS: s2897318, s2990652.                                           *
    ***************************************************************************
*/

/*
********************************************************************************
*                        Symbolic Constants: ExprType                          *
********************************************************************************
*/

// Token-Types: Primitives.
#define TT_UNDEFINED            0
#define TT_INTEGER              1
#define TT_REAL                 2

// Token-Class: Array Mask.
#define TC_ARRAY                4

// Token-Types: Arrays.
#define TT_ARRAY_INTEGER        4
#define TT_ARRAY_REAL           5

// Token-Class: Function Mask.
#define TC_FUNCTION             8

// Token-Types: Functions.
#define TT_FUNCTION_INTEGER     8
#define TT_FUNCTION_REAL        9

/*
********************************************************************************
*                              Bison Dependencies                              *
********************************************************************************
*/

// YYSTYPE: Expression data type.
typedef struct {
    unsigned    tt;         // Token-Type.
    int         vi;         // Value-Index: Index of constant value in numtab.
} exprType;

// YYSTYPE: Expression-List data type.
typedef struct {
    unsigned    length;     // Length of list.
    exprType    *list;      // Allocated list.
} exprListType;

// YYSTYPE: Variable data type.
typedef exprType varType;

// YYSTYPE: 


/*
********************************************************************************
*                           Function Prototypes: Utility                       *
********************************************************************************
*/

/* Returns a string associated with the given token-type */
const char *tokenTypeName (unsigned tt);

/*
********************************************************************************
*                           Function Prototypes: exprType                      *
********************************************************************************
*/

/* Initializes a new exprType with given token-type (tt) and value-index (vi) */
exprType initExprType (unsigned tt, unsigned vi);

/*
********************************************************************************
*                        Function Prototypes: exprListType                     *
********************************************************************************
*/

/* Initializes a new exprListType */
exprListType initExprListType (void);

/* Frees allocated memory in an exprListType */
void freeExprList(exprListType exprList);

/* Allocates a copy of the given exprType and places it in returned exprList list. */
exprListType insertExprList (exprType expr, exprListType exprList);


#endif