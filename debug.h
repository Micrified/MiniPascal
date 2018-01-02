#if !defined(DEBUG_H)
#define DEBUG_H

/*
***************************************************************************
*                  Primitive Debugger, but with Color!                    *
* AUTHORS: Charles Randolph, Joe Jones.                                   *
* SNUMBERS: s2897318, s2990652.                                           *
***************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "colors.h"

/*
***************************************************************************
*                     Type Definitions & Global Variables
***************************************************************************
*/

typedef enum {
    Structure,
    Control,
    Identifier,
    Literal,
    Operation,
    Whitespace,
    Naughty,
    EndOfFile
} SyntaxType;

extern char *yytext;
extern int yylineno;
extern int inDebug;

/*
***************************************************************************
*                    Syntactic Debug Routine Prototypes
***************************************************************************
*/

/* Prints the current token stored in yytext. Color codes according to
    abstract category */
void printToken (SyntaxType t);

/*
***************************************************************************
*                    Semantic Debug Routine Prototypes
***************************************************************************
*/

/* Prints a warning to stderr with description `msg`.
 * Accepts: Strings (%s), Integers (%d), and floats (%f) as arguments */
void printWarning (char *msg, ...);

/* Prints a error to stderr with description `msg`.
 * Accepts: Strings (%s), Integers (%d), and floats (%f) as arguments */
void printError (char *msg, ...);

#endif