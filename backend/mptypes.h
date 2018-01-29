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
#define NIL             -1    

// Type: Undefined.
#define UNDEFINED       0

// Token-Type: Primitives.
#define TT_INTEGER      1
#define TT_REAL         2

// Token-Type: Classes.
#define TC_SCALAR       3
#define TC_VECTOR       4
#define TC_ROUTINE      5

#define MAX(a,b)        ((a) > (b) ? (a) : (b))       

/*
********************************************************************************
*                               Bison Dependencies                             *
********************************************************************************
*/

// YYSTYPE: Descriptor data type.
typedef struct {
    unsigned tc;            // Token-Class.
    unsigned tt;            // Token-Type.
    unsigned vb;            // Vector-Bound.
    unsigned vl;            // Vector-Length.
} descType;

// YYSTYPE: General data type.
typedef struct {
    unsigned id;    // Identifier Index:    Index of lexeme in string table.   
    unsigned tc;    // Token-Class:         (Scalar | Vector | Routine)
    unsigned tt;    // Token-Type:          (Integer | Real | Undefined)
    unsigned ti;    // T-Index:             Vector Index for indexed variable. (T-num).
    unsigned tn;    // T-Number:            IR code number.
    unsigned vb;    // Vector-Bound:        Lower boundary of a vector.
    unsigned vl;    // Vector-Length:       Length of a vector.  
    unsigned op;    // Operator:            Saves comparison operator (hack).
} dataType;

// YYSTYPE: DataType-List data type.
typedef struct {
    unsigned length;        // Length of list.
    dataType **list;        // Allocated list of allocated dataTypes.
} dataListType;

/*
********************************************************************************
*                              DataType Prototypes                             *
********************************************************************************
*/

/* Allocates a dataType for an expression constant. */
dataType *initExprConstDataType (unsigned tn, unsigned tt);

/* Allocates a dataType for an variable expression. */
dataType *initExprVarDataType (unsigned tn, unsigned tc, unsigned tt, unsigned id);

/* Allocates a dataType for a comparative expression */
dataType *initExprCompDataType (unsigned tn, unsigned op);

/* Allocates a dataType for a scalar variable */
dataType *initVarDataType (unsigned tc, unsigned id);

/* Allocates a dataType for a indexed variable */
dataType *initVarIdxDataType (unsigned tc, unsigned id, unsigned ti);

/* Free's an allocated dataType. */
void freeDataType (dataType *dt);

/* Prints a given dataType. */
void printDataType (dataType *dt);

/*
********************************************************************************
*                            Variable-List Prototypes                          *
********************************************************************************
*/

/* Initializes a new dataList. */
dataListType initDataListType (void);

/* Frees allocated memory in a dataList */
void freeDataList(dataListType dataList);

/* Allocates a new spot for the given dataType and inserts it into the dataList type. */
dataListType insertDataType (dataType *dt, dataListType dataList);

/* Appends the first dataList to the second dataList. */
dataListType appendDataList (dataListType suffix, dataListType prefix);

/* Maps a descriptor type to a list of dataTypes. Returns dataListType instance. */
dataListType mapDescToDataList (descType desc, dataListType dataList);

/* Prints a dataListType */
void printDataListType (dataListType dataList);


#endif