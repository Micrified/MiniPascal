#if !defined(IRGEN_H)
#define IRGEN_H

#include <stdio.h>
#include <stdlib.h>
#include "mpio.h"
#include "mpascal.tab.h"

/*
***************************************************************************
*                  Intermediate-Representation Code Generator             *
* AUTHORS: Charles Randolph, Joe Jones.                                   *
* SNUMBERS: s2897318, s2990652.                                           *
***************************************************************************
*/

/*
***************************************************************************
*                   Symbolic Constants & Global Variables
***************************************************************************
*/

/* The writable file pointer */
extern FILE *irfp;

/*
***************************************************************************
*                     Expression Generation Prototypes
***************************************************************************
*/

/* Generates a T-Label for given constant. Returns T-Label num */
unsigned genConst (unsigned tt, double n);

/* Generates a T-Label for given identifier. Returns T-Label num */
unsigned genId (unsigned tt, const char *identifier);

/* Generates a T-Label for an T-indexed vector. Returns T-Label num. */
unsigned genVecIdx (unsigned tt, const char *identifier, unsigned ti, unsigned vb);

/* Generates a T-Label for a unary operation (-|+) ti. Returns T-Label num. */
unsigned genUnaryOp (unsigned tt, unsigned operator, unsigned ti);

/* Generates a T-Label for arithmetic operation. (tx op ty). Returns T-Label num. */
unsigned genArithOp (unsigned tt, unsigned operator, unsigned tx, unsigned ty);

/* Generates a T-Label for a boolean operation (tx - ty). Returns T-Label num. */
unsigned genBoolOp (unsigned tx, unsigned ty);

/*
***************************************************************************
*                     Assignment Generation Prototypes
***************************************************************************
*/

/* Generates a T-Label for a scalar assignment. */
void genScalarAssignment (const char *identifier, unsigned ti);

/* Generates a T-Label for a vector-index assignment. */
void genVectorAssignment (const char *identifier, unsigned ti, unsigned vb, unsigned te);

/*
***************************************************************************
*                    Control-Flow Generation Prototypes
***************************************************************************
*/

/* Returns the current label */
unsigned getLbl();

/* Generates a L-Label. Returns L-Label num. */
unsigned genLbl();

/* Generates a L-Label at given number. Returns L-Label num. */
unsigned genLblAt (unsigned l);

/* Reserves the next n L-Labels. */
void reserveLbl (unsigned n);

/* Generates a GOTO statement with destination L-Label */
void genGoto (unsigned l);

/* Generates a conditional statement (inverts op). Does not print newline. */
void genIf (unsigned ti, unsigned op);

/*
***************************************************************************
*                    Declaration Generation Prototypes
***************************************************************************
*/

/* Generates a scalar declaration. */
void genScalarDec (unsigned tt, const char *identifier);

/* Generates a vector decalaration. */
void genVectorDec (unsigned tt, unsigned n, const char *identifier);

/*
***************************************************************************
*                    Structural Generation Prototypes
***************************************************************************
*/

/* Generates the main program header and opening brace */
void genMainHeader ();

/* Generates the return statement and closing brace for main */
void genMainEnd ();

/*
***************************************************************************
*                 Library Procedure Generation Prototypes
***************************************************************************
*/

/* Generates a statement to scan in values. */
void genReadLn (dataListType dataList);

/* Generates a statement to print values. */
void genWriteLn (dataListType dataList);


#endif