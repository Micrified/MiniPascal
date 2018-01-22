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

// Represents an invalid table number index.
#define NIL         -1    

// Type: Undefined.
#define UNDEFINED    0

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

// YYSTYPE: Descriptor data type.
typedef struct {
    unsigned tc;            // Token-Class.
    unsigned tt;            // Token-Type.
} descType;

// YYSTYPE: Variable data type.
typedef struct {
    unsigned tc;            // Token-Class.
    unsigned tt;            // Token-Type.
    unsigned vi;            // Value-Index: Index of constant value in numtab.
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
*                               Variable Prototypes                            *
********************************************************************************
*/

/* Initializes a new varType with the given token-class (tc), token-type (tt) and 
 * identifier-index (id).
*/
varType initVarType (unsigned tc, unsigned tt, unsigned id);

/* Initializes a new expression varType with the given tokenc-class (tc), 
 * token-type (tt), and value-index (vi). This type has no symbol table entry.
*/
varType initExprVarType (unsigned tc, unsigned tt, unsigned vi);

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

/* Appends the first varListType list to second varListType */
varListType appendVarList (varListType suffix, varListType prefix);

#endif