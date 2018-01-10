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

// Arithmetic Operator Tokens.
%token MP_ADDOP 
%token MP_MULOP 
%token MP_DIVOP
%token MP_MODOP

// Relational Operator Tokens.
%token MP_RELOP_LT
%token MP_RELOP_LE
%token MP_RELOP_EQ
%token MP_RELOP_GE
%token MP_RELOP_GT
%token MP_RELOP_NE

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
  unsigned      num;
  exprType      expr;
  exprListType  exprList;
  varType       var;
  varListType   varList;
}

// Nonterminal return type rules.
%type <num> type standardType identifier statementList optionalStatements
%type <expr> factor term simpleExpression expression 
%type <exprList> expressionList
%type <var> variable
%type <varList> identifierList parameterList arguments declarations
// Starting Grammar Rule.
%start program

%%

/*
********************************************************************************
*                                   Grammar
********************************************************************************
*/

program : MP_PROGRAM MP_ID MP_POPEN identifierList MP_PCLOSE MP_SCOLON declarations { installVarList($7); } subprogramDeclarations compoundStatement MP_FSTOP MP_EOF { printf("ACCEPTED\n"); exit(0); }
        ;

identifierList  : identifier                                         { $$ = insertVarType(initVarType(TT_UNDEFINED, $1), initVarListType()); }
                | identifierList MP_COMMA identifier                 { $$ = insertVarType(initVarType(TT_UNDEFINED, $3), $1); }
                ;

declarations  : declarations MP_VAR identifierList MP_COLON type MP_SCOLON  { $$ = appendVarList(mapTokenTypeToVarList($5, $3), $1); }
              |                                                             { $$ = initVarListType(); }
              ;

type  : standardType                                                          { $$ = $1; }
      | MP_ARRAY MP_BOPEN MP_INTEGER MP_ELLIPSES MP_INTEGER MP_BCLOSE MP_OF standardType  { if ($8 == TT_INTEGER) {return TT_ARRAY_INTEGER;} else {return TT_ARRAY_REAL;} }
      ;

standardType  : MP_TYPE_INTEGER                                               { $$ = TT_INTEGER; }
              | MP_TYPE_REAL                                                  { $$ = TT_REAL; }
              ;

subprogramDeclarations  : subprogramDeclarations subprogramDeclaration MP_SCOLON
                        |
                        ;

subprogramDeclaration : subprogramHead declarations compoundStatement { installVarList($2); decrementScopeLevel(); }
                      ;

subprogramHead  : MP_FUNCTION identifier arguments MP_COLON standardType MP_SCOLON  { installFunction($2, $5); incrementScopeLevel();  installFunctionArgs($2, $3); }
                | MP_PROCEDURE identifier arguments MP_SCOLON                       { installProcedure($2); incrementScopeLevel(); installProcedureArgs($2, $3); }                      
                ;

arguments : MP_POPEN parameterList MP_PCLOSE                      { $$ = $2; }
          |                                                       { $$ = initVarListType(); }
          ;

parameterList : identifierList MP_COLON type                      { $$ = mapTokenTypeToVarList($3, $1); }                     
              | parameterList MP_SCOLON identifierList MP_COLON type { $$ = appendVarList(mapTokenTypeToVarList($5, $3), $1); }
              ;

compoundStatement : MP_BEGIN optionalStatements MP_END            { if ($2 == 0) printWarning("Empty compound statement!"); }            
                  ;

optionalStatements  : statementList                               { $$ = $1; }                              
                    |                                             { $$ = 0; }                                               
                    ;

statementList : statement                                         { $$ = 1; }                    
              | statementList MP_SCOLON statement                 { $$ = $1 + 1; } 
              ;

statement : variable MP_ASSIGNOP expression                       { resolveAssignment($1, $3); }                 
          | procedureStatement
          | compoundStatement
          | MP_IF expression MP_THEN statement MP_ELSE statement  { verifyGuardExpr($2); }
          | MP_WHILE expression MP_DO statement                   { verifyGuardExpr($2); }             
          ;

variable  : identifier                                            { $$ = initVarType(getTypeElseError(identifierAtIndex($1)), $1); }                                                                                      
          | identifier MP_BOPEN expression MP_BCLOSE              { requireExprType(TT_INTEGER, $3); $$ = initVarType(resolveTypeClass(identifierAtIndex($1), TC_ARRAY), $1); }                  

procedureStatement  : identifier                                 
                    | identifier MP_POPEN expressionList MP_PCLOSE { resolveTypeClass(identifierAtIndex($1), TC_PROCEDURE); verifyRoutineArgs($1, $3);}
                    ;

expressionList  : expression                                      { $$ = insertExprList($1, initExprListType()); }                                                              
                | expressionList MP_COMMA expression              { $$ = insertExprList($3, $1); }                
                ;

expression  : simpleExpression                                    { $$ = $1; }
            | simpleExpression MP_RELOP_LT simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_LT, $1, $3); }
            | simpleExpression MP_RELOP_LE simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_LE, $1, $3); }
            | simpleExpression MP_RELOP_EQ simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_EQ, $1, $3); }
            | simpleExpression MP_RELOP_GE simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_GE, $1, $3); }
            | simpleExpression MP_RELOP_GT simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_GT, $1, $3); }
            | simpleExpression MP_RELOP_NE simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_NE, $1, $3); }
            ;

simpleExpression  : term                                          { $$ = $1; }
                  | sign term                                     { $$ = $2; }
                  | simpleExpression MP_ADDOP term                { $$ = resolveArithmeticOperation(MP_ADDOP, $1, $3); }
                  ;

term  : factor                                                    { $$ = $1; }
      | term MP_MULOP factor                                      { $$ = resolveArithmeticOperation(MP_MULOP, $1, $3); }
      | term MP_DIVOP factor                                      { $$ = resolveArithmeticOperation(MP_DIVOP, $1, $3); }
      | term MP_MODOP factor                                      { $$ = resolveArithmeticOperation(MP_MODOP, $1, $3); }
      ;

factor  : identifier                                              { $$ = initExprType(getTypeElseInstall(identifierAtIndex($1), TT_UNDEFINED), NIL); }
        | identifier MP_POPEN expressionList MP_PCLOSE            { $$ = initExprType(resolveTypeClass(identifierAtIndex($1), TC_FUNCTION), NIL); verifyRoutineArgs($1, $3); }
        | identifier MP_BOPEN expression MP_BCLOSE                { requireId($1, TC_VECTOR); requireExprType(TT_INTEGER, $3); $$ = initExprType(getIdTokenType($1, TC_VECTOR))
                                                                    
                                                                    $$ = initExprType(resolveTypeClass(identifierAtIndex($1), TC_ARRAY), NIL); 
                                                                  } 
        | MP_INTEGER                                              { $$ = initExprType(TT_INTEGER, installNumber(atof(yytext))); }
        | MP_REAL                                                 { $$ = initExprType(TT_REAL, installNumber(atof(yytext))); }
        | MP_POPEN expression MP_PCLOSE                           { $$ = $2; }
        ;

identifier : MP_ID                                                { $$ = installId(yytext); }

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

  // Initialize support structures.
  initStringTable();
  initNumberTable();

  // Read program flags
  if (argc > 1) {
    parseArguments(argc, argv);
  }

  // Perform Syntactic + Symantic Analysis.
  yyparse();

  // Free allocate memory.
  freeNumberTable();
  freeStringTable();
  freeSymbolTableEntries();
  
  return EXIT_SUCCESS;
}

