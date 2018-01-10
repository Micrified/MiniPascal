#if !defined(MP_TYPES_H)
#define MP_TYPES_H

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
*                               Symbolic Constants                             *
********************************************************************************
*/

// Token-Type: Undefined.
#define TT_UNDEFINED    0

// Token-Type: Primitives.
#define TT_INTEGER      1
#define TT_REAL         2

// Token-Type: Classes.
#define TC_SCALAR       3
#define TC_VECTOR       4
#define TC_ROUTINE      5

/*
********************************************************************************
*                               Bison Dependencies                             *
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
    unsigned tc;            // Token-Class.
    unsigned id;            // Identifier-Index: Index of the identifier in strtab.
} varType;

// YYSTYPE: Variable-List data type.
typedef struct {
    unsigned length;        // Length of list.
    varType *list;          // Allocated list.
} varListType;

/*
********************************************************************************
*                              Conversion Prototypes                           *
********************************************************************************
*/

/* Returns a string associated with the given token-type */
const char *tokenTypeName (unsigned tt);

/* Returns a string associated with the given token-class */
const char *tokenClassName (unsigned tc);

/*
********************************************************************************
*                              Expression Prototypes                           *
********************************************************************************
*/

/* Initializes a new exprType with given token-type (tt) and value-index (vi) */
exprType initExprType (unsigned tt, unsigned vi);

/*
********************************************************************************
*                            Expression-List Prototypes                        *
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
*                               Variable Prototypes                            *
********************************************************************************
*/

/* Initializes a new varType with the given token-class (tc) and identifier-index (id) */
varType initVarType (unsigned tc, unsigned id);

/*
********************************************************************************
*                            Variable-List Prototypes                          *
********************************************************************************
*/

/* Initializes a new varListType */
varListType initVarListType (void);

/* Frees allocated memory in a varListType */
void freeVarList(varListType varList);

/* Allocates a copy of the given varType and places it in returned varList list. */
varListType insertVarType (varType var, varListType varList);

/* Maps the given token-class to the list of varTypes in the varList list. */
varListType mapTokenTypeToVarList (unsigned tc, varListType varList);

/* Appends the first varListType list to second varListType */
varListType appendVarList (varListType suffix, varListType prefix);

#endif