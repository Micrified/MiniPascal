/*
  ***************************************************************************
  *                      Mini Pascal Syntax Analysis                        *
  * AUTHORS: Charles Randolph, Joe Jones.                                   *
  * SNUMBERS: s2897318, s2990652.                                           *
  ***************************************************************************
*/
    
/*
********************************************************************************
*                                 Verbatim C 
********************************************************************************
*/

%{

#include <stdio.h>
#include <stdlib.h>

/* Custom Routine Imports */
#include "semantics.h"  // Semantic Checking Routines.

/* Variables local to lex.yy.c */
extern int inDebug;
extern int yylex();
extern int yylineno;
extern char *yytext;

/* Handler for Bison parse errors */
int yyerror(char *s) {
  printf("PARSE ERROR (%d)\n", yylineno);
  exit(EXIT_SUCCESS);
}

%}

/*
********************************************************************************
*                               Token Declarations
********************************************************************************
*/

// Structural Tokens.
%token MP_PROGRAM
%token MP_BEGIN
%token MP_END
%token MP_EOF

// Control Flow Tokens.
%token MP_WHILE
%token MP_DO
%token MP_IF
%token MP_THEN
%token MP_ELSE

// Operator Tokens.
%token MP_RELOP
%token MP_ADDOP 
%token MP_MULOP 
%token MP_DIVOP
%token MP_MODOP

// Primitive Tokens.
%token MP_ID
%token MP_INTEGER
%token MP_REAL

// Type Tokens.
%token MP_TYPE_INTEGER
%token MP_TYPE_REAL
%token MP_FUNCTION
%token MP_PROCEDURE
%token MP_ARRAY
%token MP_VAR

// Assignment/Relation Tokens.
%token MP_ASSIGNOP 
%token MP_OF

// Punctuation Tokens.
%token MP_COMMA
%token MP_POPEN             
%token MP_PCLOSE
%token MP_BOPEN
%token MP_BCLOSE
%token MP_COLON
%token MP_SCOLON
%token MP_FSTOP
%token MP_ELLIPSES

// Unexpected Tokens.
%token MP_WTF

/*
********************************************************************************
*                               Bison Declarations
********************************************************************************
*/

// YYSTYPE Dependencies.
%code requires {
  #include "mptypes.h"    // Types used in symantic checker.
  #include "strtab.h"     // String Table.
  #include "numtab.h"     // Number Table.
  #include "symtab.h"     // Symbol Table.
}

// Bison YYSTYPE Declarations.
%union {
  exprType expr;
}

// Nonterminal return type rules.
%type <expr> factor term simpleExpression expression

// Starting Grammar Rule.
%start program

%%

/*
********************************************************************************
*                                   Grammar
********************************************************************************
*/

program : MP_PROGRAM MP_ID MP_POPEN identifierList MP_PCLOSE MP_SCOLON declarations subprogramDeclarations compoundStatement MP_FSTOP MP_EOF { printf("ACCEPTED\n"); exit(0); }
        ;

identifierList  : MP_ID
                | identifierList MP_COMMA MP_ID
                ;

declarations  : declarations MP_VAR identifierList MP_COLON type MP_SCOLON
              |
              ;

type  : standardType
      | MP_ARRAY MP_BOPEN MP_INTEGER MP_ELLIPSES MP_INTEGER MP_BCLOSE MP_OF standardType
      ;

standardType  : MP_TYPE_INTEGER
              | MP_TYPE_REAL
              ;

subprogramDeclarations  : subprogramDeclarations subprogramDeclaration MP_SCOLON
                        |
                        ;

subprogramDeclaration : subprogramHead declarations compoundStatement
                      ;

subprogramHead  : MP_FUNCTION MP_ID arguments MP_COLON standardType MP_SCOLON
                | MP_PROCEDURE MP_ID arguments MP_SCOLON
                ;

arguments : MP_POPEN parameterList MP_PCLOSE
          |
          ;

parameterList : identifierList MP_COLON type
              | parameterList MP_SCOLON identifierList MP_COLON type
              ;

compoundStatement : MP_BEGIN optionalStatements MP_END
                  ;

optionalStatements  : statementList
                    |
                    ;

statementList : statement
              | statementList MP_SCOLON statement
              ;

statement : variable MP_ASSIGNOP expression
          | procedureStatement
          | compoundStatement
          | MP_IF expression MP_THEN statement MP_ELSE statement
          | MP_WHILE expression MP_DO statement
          ;

variable  : MP_ID
          | MP_ID MP_BOPEN expressionList MP_BCLOSE

procedureStatement  : MP_ID
                    | MP_ID MP_POPEN expressionList MP_PCLOSE
                    ;

expressionList  : expression 
                | expressionList MP_COMMA expression 
                ;

expression  : simpleExpression                            { $$ = $1; }
            | simpleExpression MP_RELOP simpleExpression  { $$ = resolveBooleanOperation($1, $3); }
            ;

simpleExpression  : term                                  { $$ = $1; }
                  | sign term                             { $$ = $2; }
                  | simpleExpression MP_ADDOP term        { $$ = resolveArithmeticOperation(MP_ADDOP, $1, $3); }
                  ;

term  : factor                                            { $$ = $1; }
      | term MP_MULOP factor                              { $$ = resolveArithmeticOperation(MP_MULOP, $1, $3); }
      | term MP_DIVOP factor                              { $$ = resolveArithmeticOperation(MP_DIVOP, $1, $3); }
      | term MP_MODOP factor                              { $$ = resolveArithmeticOperation(MP_MODOP, $1, $3); }
      ;

factor  : MP_ID                                           { $$ = (exprType){.tt = getTypeElseInstall(yytext, TT_UNDEFINED), .vi = NIL}; }
        | MP_ID MP_POPEN expressionList MP_PCLOSE         { $$ = (exprType){.tt = resolveTypeClass(yytext, TC_FUNCTION), .vi = NIL}; }
        | MP_ID MP_BOPEN expression MP_BCLOSE             { $$ = (exprType){.tt = resolveTypeClass(yytext, TC_ARRAY), .vi = NIL}; }
        | MP_INTEGER                                      { $$ = (exprType){.tt = TT_INTEGER, .vi = installNumber(atof(yytext))}; }
        | MP_REAL                                         { $$ = (exprType){.tt = TT_REAL, .vi = installNumber(atof(yytext))}; }
        | MP_POPEN expression MP_PCLOSE                   { $$ = $2; }
        ;

sign  : MP_ADDOP
      ;

%%

/*
 ********************************************************************************
 *                                  C Routines
 ********************************************************************************
 */

void parseArguments (int argc, char *argv[]) {
  char *arg;

  while (--argc && *(arg = *++argv) == '-') {
    switch (*++arg) {
      case 'd':
        inDebug = 1; 
        break;
      default:
        fprintf(stderr, "Unknown argument \"-%s\"!\n", arg);
        exit(EXIT_FAILURE);
        break;
    }
  }
}

int main(int argc, char *argv[]) {

  // Read program flags
  if (argc > 1) {
    parseArguments(argc, argv);
  }

  // Perform Syntactic + Symantic Analysis.
  yyparse();
  
  return EXIT_SUCCESS;
}

