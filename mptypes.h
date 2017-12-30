#if !defined(MPTYPES_H)
#define MPTYPES_H

/*
    ***************************************************************************
    *                   Types used in MiniPascal Compiler                     *
    * AUTHORS: Charles Randolph, Joe Jones.                                   *
    * SNUMBERS: s2897318, s2990652.                                           *
    ***************************************************************************
*/

/*
********************************************************************************
*                               Symbolic Constants                             *
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
    unsigned    tt;        // Token-Type.
    int         vi;        // Value-Index: Index of constant value in numtab.
} exprType;

#endif