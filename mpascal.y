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

/* Variables local to lex.yy.c */
extern int yylex();
extern int yylineno;

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

// Primitive Tokens.
%token MP_ID
%token MP_NUM

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

// Bison Declarations.
%union {
  float fval;
  char *strtabptr;
}

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
      | MP_ARRAY MP_BOPEN MP_NUM MP_ELLIPSES MP_NUM MP_BCLOSE MP_OF standardType
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

expression  : simpleExpression
            | simpleExpression MP_RELOP simpleExpression
            ;

simpleExpression  : term
                  | sign term
                  | simpleExpression MP_ADDOP term
                  ;

term  : factor
      | term MP_MULOP factor
      ;

factor  : MP_ID
        | MP_ID MP_POPEN expressionList MP_PCLOSE
        | MP_ID MP_BOPEN expression MP_BCLOSE
        | MP_NUM
        | MP_POPEN expression MP_PCLOSE
        ;

sign  : MP_ADDOP
      ;

%%

/*
 ********************************************************************************
 *                                  C Routines
 ********************************************************************************
 */

int main(int argc, char *argv[]) {

  yyparse();
  
  return EXIT_SUCCESS;
}

