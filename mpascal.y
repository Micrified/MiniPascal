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
  descType      desc;
  exprType      expr;
  exprListType  exprList;
  varType       var;
  varListType   varList;
}

// Nonterminal return type rules.
%type <num> standardType identifier statementList optionalStatements
%type <desc> type
%type <expr> factor term simpleExpression expression 
%type <exprList> expressionList
%type <var> variable subprogramHead
%type <varList> identifierList parameterList arguments declarations
// Starting Grammar Rule.
%start program

%%

/*
********************************************************************************
*                                   Grammar
********************************************************************************
*/

program : MP_PROGRAM MP_ID MP_POPEN identifierList MP_PCLOSE MP_SCOLON declarations { /* Install declarations in symbol-table */ installVarList($7); } 
          subprogramDeclarations 
          compoundStatement 
          MP_FSTOP MP_EOF 
          { printf("ACCEPTED\n"); exit(0); }
        ;

identifierList  : identifier                                          { /* Insert new varType for identifier in a new varList */
                                                                        $$ = insertVarType(initVarType(UNDEFINED, UNDEFINED, $1), initVarListType()); 
                                                                      }
                | identifierList MP_COMMA identifier                  { /* Insert varType for identifier into varList */
                                                                        $$ = insertVarType(initVarType(UNDEFINED, UNDEFINED, $3), $1); 
                                                                      }
                ;

declarations  : declarations MP_VAR identifierList MP_COLON type MP_SCOLON  { /* Apply token-class and token-types to varType entries in identifierList.
                                                                                 Then append new identifierList to varList of other declarations. */
                                                                              $$ = appendVarList(mapDescToVarList($5, $3), $1); 
                                                                            }
              |                                                             { /* Initialize an empty varList */
                                                                              $$ = initVarListType(); 
                                                                            }
              ;

type  : standardType                                                                      { /* Return class scalar of standard token-type */
                                                                                            $$ = (descType){.tc = TC_SCALAR, .tt = $1}; 
                                                                                          }
      | MP_ARRAY MP_BOPEN MP_INTEGER MP_ELLIPSES MP_INTEGER MP_BCLOSE MP_OF standardType  { /* Return class vector of standard token-type */ 
                                                                                            $$ = (descType){.tc = TC_VECTOR, .tt = $8}; 
                                                                                          }
      ;

standardType  : MP_TYPE_INTEGER                                                           { $$ = TT_INTEGER; }
              | MP_TYPE_REAL                                                              { $$ = TT_REAL; }
              ;

subprogramDeclarations  : subprogramDeclarations subprogramDeclaration MP_SCOLON
                        |
                        ;

subprogramDeclaration : subprogramHead declarations { /* Install declarations in symbol-table */
                                                      installVarList($2); 
                                                    } 
                        compoundStatement           { /* If routine is a function, warn if return variable uninitialized  */
                                                      if ($1.tt != UNDEFINED) {
                                                        verifyFunctionReturnValue($1.id);
                                                      }
                                                      /* Drop scope level after end of body */
                                                      decrementTableScope(); 
                                                    }
                      ;

subprogramHead  : MP_FUNCTION identifier arguments MP_COLON standardType MP_SCOLON  { /* Attempt to install function and arguments */
                                                                                      if (installRoutine($2, $5)) {
                                                                                        incrementTableScope();
                                                                                        installRoutineArgs($2, $3);
                                                                                      }
                                                                                      freeVarList($3);
                                                                                      $$ = initVarType(TC_ROUTINE, $5, $2);
                                                                                    }
                | MP_PROCEDURE identifier arguments MP_SCOLON                       { /* Attempt to install procedure and arguments */
                                                                                      if (installRoutine($2, UNDEFINED)) {
                                                                                        incrementTableScope();
                                                                                        installRoutineArgs($2, $3);
                                                                                      }
                                                                                      freeVarList($3);
                                                                                      $$ = initVarType(TC_ROUTINE, UNDEFINED, $2);
                                                                                    }                      
                ;

arguments : MP_POPEN parameterList MP_PCLOSE                      { $$ = $2; }
          |                                                       { $$ = initVarListType(); }
          ;

parameterList : identifierList MP_COLON type                          { /* Map a descType (token-class,token-type) to varList of identifiers */
                                                                        $$ = mapDescToVarList($3, $1); 
                                                                      }                     
              | parameterList MP_SCOLON identifierList MP_COLON type  { /* Map descType to identifierList, and append to parameter varList */
                                                                        $$ = appendVarList(mapDescToVarList($5, $3), $1); 
                                                                      }
              ;

compoundStatement : MP_BEGIN optionalStatements MP_END            { /* Print a warning if a compound statment is left empty */
                                                                    if ($2 == 0) {
                                                                      printWarning("Empty compound statement!"); 
                                                                    }
                                                                  }            
                  ;

optionalStatements  : statementList                               { $$ = $1; }                              
                    |                                             { $$ = 0; }                                               
                    ;

statementList : statement                                         { $$ = 1; }                    
              | statementList MP_SCOLON statement                 { $$ = $1 + 1; } 
              ;

statement : variable MP_ASSIGNOP expression                       { /* Verify expression may be assigned to variable */
                                                                    verifyAssignment($1, $3); 
                                                                  }                 
          | procedureStatement
          | compoundStatement
          | MP_IF expression MP_THEN statement MP_ELSE statement  { /* Verify boolean guard expression is of Integer token-type */
                                                                    verifyGuardExpr($2); 
                                                                  }
          | MP_WHILE expression MP_DO statement                   { verifyGuardExpr($2); }             
          ;

variable  : identifier                                            { /* Expect scalar id entry in symbol-table. Else install as undefined */
                                                                    if (existsId($1, TC_SCALAR)) {
                                                                      $$ = initVarTypeFromId($1, TC_SCALAR); 
                                                                    } else {
                                                                      $$ = initVarType(UNDEFINED, UNDEFINED, $1);
                                                                    }
                                                                  }                                                                                     
          | identifier MP_BOPEN expression MP_BCLOSE              { /* Expect vector id entry in symbol-table. Else install as undefined */
                                                                    if (existsId($1, TC_VECTOR)) {
                                                                      requireExprType(TT_INTEGER, $3); 
                                                                      $$ = initVarType(TC_VECTOR, getIdTokenType($1, TC_VECTOR), $1);
                                                                    }
                                                                  }                  

procedureStatement  : identifier                                 
                    | identifier MP_POPEN expressionList MP_PCLOSE { /* Verify call to routine is valid */
                                                                      if (existsId($1, TC_ROUTINE)) { 
                                                                        verifyRoutineArgs($1, $3); 
                                                                      }
                                                                      freeExprList($3);
                                                                    }
                    ;

expressionList  : expression                                      { $$ = insertExprList($1, initExprListType()); }                                                              
                | expressionList MP_COMMA expression              { $$ = insertExprList($3, $1); }                
                ;

expression  : simpleExpression                                    { $$ = $1; }
            | simpleExpression MP_RELOP_LT simpleExpression       { /* Check boolean expression types and attempt to resolve/fold expression */
                                                                    $$ = resolveBooleanOperation(MP_RELOP_LT, $1, $3); 
                                                                  }
            | simpleExpression MP_RELOP_LE simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_LE, $1, $3); }
            | simpleExpression MP_RELOP_EQ simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_EQ, $1, $3); }
            | simpleExpression MP_RELOP_GE simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_GE, $1, $3); }
            | simpleExpression MP_RELOP_GT simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_GT, $1, $3); }
            | simpleExpression MP_RELOP_NE simpleExpression       { $$ = resolveBooleanOperation(MP_RELOP_NE, $1, $3); }
            ;

simpleExpression  : term                                          { $$ = $1; }
                  | sign term                                     { $$ = $2; }
                  | simpleExpression MP_ADDOP term                { /* Check arithmetic expression types and attempt to resolve/fold expression */
                                                                    $$ = resolveArithmeticOperation(MP_ADDOP, $1, $3); 
                                                                  }
                  ;

term  : factor                                                    { $$ = $1; }
      | term MP_MULOP factor                                      { $$ = resolveArithmeticOperation(MP_MULOP, $1, $3); }
      | term MP_DIVOP factor                                      { /* Check expression types and attempt to resolve/fold expression. Check for div-zero */
                                                                    $$ = resolveArithmeticOperation(MP_DIVOP, $1, $3); 
                                                                  }
      | term MP_MODOP factor                                      { $$ = resolveArithmeticOperation(MP_MODOP, $1, $3); }
      ;

factor  : identifier                                              { /* Verify scalar factor exists. Warn if uninitialized */
                                                                    if (existsId($1, TC_SCALAR)) {
                                                                      isInitialized($1, TC_SCALAR);
                                                                      $$ = initExprType(getIdTokenType($1, TC_SCALAR), NIL);
                                                                    } else { 
                                                                      $$ = initExprType(UNDEFINED, NIL); 
                                                                    }
                                                                  }
        | identifier MP_POPEN expressionList MP_PCLOSE            { /* Verify routine factor exists, and has proper arguments */
                                                                    if (existsId($1, TC_ROUTINE)) { 
                                                                      verifyRoutineArgs($1, $3); 
                                                                      $$ = initExprType(getIdTokenType($1, TC_ROUTINE), NIL); 
                                                                    } else {
                                                                      $$ = initExprType(UNDEFINED, NIL);
                                                                    }
                                                                    freeExprList($3);
                                                                  }
        | identifier MP_BOPEN expression MP_BCLOSE                { /* Verify vector factor exists, and indexing expression is valid */
                                                                    if (existsId($1, TC_VECTOR)) {
                                                                      requireExprType(TT_INTEGER, $3);
                                                                      $$ = initExprType(getIdTokenType($1, TC_VECTOR), NIL);
                                                                    } else {
                                                                      $$ = initExprType(UNDEFINED, NIL);
                                                                    }
                                                                  } 
        | MP_INTEGER                                              { $$ = initExprType(TT_INTEGER, installNumber(atof(yytext))); }
        | MP_REAL                                                 { $$ = initExprType(TT_REAL, installNumber(atof(yytext))); }
        | MP_POPEN expression MP_PCLOSE                           { $$ = $2; }
        ;

identifier : MP_ID                                                { /* Dedicated rule is necessary to properly install token lexemes */
                                                                    $$ = installId(yytext); 
                                                                  }

sign  : MP_ADDOP
      ;


%%

/*
 ********************************************************************************
 *                                  C Routines
 ********************************************************************************
 */

/* Parses program argument vector for program flags */
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

  // Initialize supporting tables.
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
  freeSymbolTables();
  
  return EXIT_SUCCESS;
}

