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

// Token-Types: Undefined.
#define TT_UNDEFINED            0

// Token-Types: Primitives.
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

// Token-Class: Procedures.
#define TC_PROCEDURE            16

// Token-Types: Procedures.
#define TT_PROCEDURE            16

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
typedef struct {
    unsigned tt;            // Token-Type.
    unsigned id;            // Identifier-Index: Index of the identifier in strtab.
} varType;

// YYSTYPE: Variable-List data type.
typedef struct {
    unsigned length;        // Length of list.
    varType *list;          // Allocated list.
} varListType;


/*
********************************************************************************
*                           Function Prototypes: Utility                       *
********************************************************************************
*/

/* Returns a string associated with the given token-type */
const char *tokenTypeName (unsigned tt);

/* Returns a string associated with the given token-class */
const char *tokenClassName (unsigned tc);

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

/*
********************************************************************************
*                        Function Prototypes: varType                          *
********************************************************************************
*/

/* Initializes a new varType with the given token-type (tt) and identifier-index (id) */
varType initVarType (unsigned tt, unsigned id);

/*
********************************************************************************
*                      Function Prototypes: varListType                        *
********************************************************************************
*/

/* Initializes a new varListType */
varListType initVarListType (void);

/* Frees allocated memory in a varListType */
void freeVarList(varListType varList);

/* Allocates a copy of the given varType and places it in returned varList list. */
varListType insertVarType (varType var, varListType varList);

/* Maps the given token-type to the list of varTypes in the varList list. */
varListType mapTokenTypeToVarList (unsigned tt, varListType varList);

/* Appends the first varListType list to second varListType */
varListType appendVarList (varListType suffix, varListType prefix);


#endif