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
#include "colors.h"

/*
***************************************************************************
*                     Type Definitions & Variables
***************************************************************************
*/

typedef enum {
    Structure,
    Control,
    Identifier,
    Literal,
    Operation,
    Whitespace,
    Naughty
} SyntaxType;

extern char *yytext;
extern int yylineno;

/*
***************************************************************************
*                               Prototypes
***************************************************************************
*/

/* Print's the current token stored in yytext. Color codes according to
    abstract category */
void printToken (SyntaxType t);

#endif