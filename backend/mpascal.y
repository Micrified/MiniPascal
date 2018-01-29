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
#include "mpio.h"       // IO Handler (code generation).
#include "irgen.h"

/* Variables local to lex.yy.c */
extern int yylex();
extern void yylex_destroy();
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
%token MP_SUBOP 
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

// Reserved Tokens.
%token MP_READLN
%token MP_WRITELN

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
  dataType      *data;
  dataListType  dataList; 
}

// Nonterminal return type rules.
%type <num> standardType sign identifier 
%type <desc> type subprogramHead
%type <data> variable expression factor term simpleExpression
%type <dataList> declarations identifierList expressionList parameterList arguments

// Starting Grammar Rule.
%start program

%%

/*
********************************************************************************
*                                   Grammar
********************************************************************************
*/

program : MP_PROGRAM MP_ID MP_POPEN identifierList    { freeDataList($4); /* Ignore program parameters */ } 
          MP_PCLOSE MP_SCOLON declarations            {
                                                        /* Installing all program declarations */
                                                        for (int i = 0; i < $8.length; i++) {
                                                          dataType *d = $8.list[i];

                                                          /* Install each entry into the symbol table. */
                                                          installIdEntry(d->id, d->tc, d->tt, d->vb, d->vl);

                                                          /* Generate an appropriate declaration (Vector | Scalar) */
                                                          if (d->tc == TC_VECTOR) {
                                                            genVectorDec(d->tt, d->vl, identifierAtIndex(d->id));
                                                          } else {
                                                            genScalarDec(d->tt, identifierAtIndex(d->id));
                                                          }
                                                        } 
                                                        freeDataList($8); 
                                                      } 
          subprogramDeclarations                      { /* Generate the main program header and opening brace for C */
                                                        genMainHeader(); 
                                                      }
          compoundStatement                           { /* Generate the return statement and closing brace for main */
                                                        genMainEnd(); 
                                                      } 
          MP_FSTOP MP_EOF                             { /* End of File: Accept the program! */
                                                        YYACCEPT; 
                                                      }
        ;

identifierList  : identifier                                          { /* Insert new dataType for identifier in a new dataList */
                                                                        $$ = insertDataType(initVarDataType(UNDEFINED, $1), initDataListType()); 
                                                                      }
                | identifierList MP_COMMA identifier                  { /* Insert dataType for identifier into dataList */
                                                                        $$ = insertDataType(initVarDataType(UNDEFINED, $3), $1);
                                                                      }
                ;

declarations  : declarations MP_VAR identifierList MP_COLON type MP_SCOLON  { /* Apply token-class and token-types to dattype entries in identifierList.
                                                                                 Then append new identifierList to dataList of other declarations. */
                                                                              $$ = appendDataList(mapDescToDataList($5, $3), $1);
                                                                            }
              |                                                             { /* Initialize an empty dataList */
                                                                              $$ = initDataListType();
                                                                            }
              ;

type  : standardType                                                                      { /* Return class scalar of standard token-type */
                                                                                            $$ = (descType){.tc = TC_SCALAR, .tt = $1, .vb = 0, .vl = 0}; 
                                                                                          }
      | MP_ARRAY MP_BOPEN MP_INTEGER                                                      { $<num>$ = atoi(yytext); } 
        MP_ELLIPSES MP_INTEGER                                                            { $<num>$ = atoi(yytext); } 
        MP_BCLOSE MP_OF standardType                                                      { $$ = (descType){.tc = TC_VECTOR, .tt = $10, .vb = $<num>4, .vl = ($<num>7 - $<num>4 + 1)}; }
      ;

standardType  : MP_TYPE_INTEGER                                                           { $$ = TT_INTEGER; }
              | MP_TYPE_REAL                                                              { $$ = TT_REAL; }
              ;

subprogramDeclarations  : subprogramDeclarations subprogramDeclaration MP_SCOLON
                        |
                        ;

subprogramDeclaration : subprogramHead                                                    { /* Generate the opening brace after the header */
                                                                                             genBlockStart();  
                                                                                          }
                        declarations                                                      { 
                                                                                            /* If the routine is a function, install a local scalar declaration to return */ 
                                                                                            if ($1.tc != UNDEFINED) {
                                                                                              installIdEntry($1.tt, TC_SCALAR, $1.tc, 0, 0);
                                                                                              genScalarDec($1.tc, identifierAtIndex($1.tt));
                                                                                            }

                                                                                            /* Install all remaining local variable declarations. */
                                                                                            for (int i = 0; i < $3.length; i++) {
                                                                                              dataType *d = $3.list[i];

                                                                                              /* Install each entry into the symbol table */
                                                                                              installIdEntry(d->id, d->tc, d->tt, d->vb, d->vl);

                                                                                              /* Generate an appropriate declaration (Vector | Scalar) */
                                                                                              if (d->tc == TC_VECTOR) {
                                                                                                genVectorDec(d->tt, d->vl, identifierAtIndex(d->id));
                                                                                              } else {
                                                                                                genScalarDec(d->tt, identifierAtIndex(d->id));
                                                                                              }
                                                                                            }

                                                                                            /* Clean up allocated memory */
                                                                                            freeDataList($3);
                                                                                          }  
                        compoundStatement                                                 { /* Generate the return statement, close the block, and decrement table scope */
                                                                                            genReturn($1.tc, $1.tt); genBlockEnd(); decrementTableScope();
                                                                                          }
                      ;

subprogramHead  : MP_FUNCTION identifier arguments MP_COLON standardType MP_SCOLON        { /* Install function header */ 
                                                                                            genRoutineDec($5, identifierAtIndex($2), $3);

                                                                                            /* Install entry: Vector-Count (vc) stores arg count */
                                                                                            installIdEntry($2, TC_ROUTINE, $5, 0, $3.length);

                                                                                            /* Increment table scope before installing arguments */
                                                                                            incrementTableScope();

                                                                                            /* Install arguments */
                                                                                            for (int i = 0; i < $3.length; i++) {
                                                                                              dataType *d = $3.list[i];
                                                                                              
                                                                                              /* Install each entry into the symbol table */
                                                                                              installIdEntry(d->id, d->tc, d->tt, d->vb, d->vl);
                                                                                            }

                                                                                            /* Free allocated memory */
                                                                                            freeDataList($3);

                                                                                            /* Custom DescType (.tc = Return Token-Type, .tt = Routine Name) */
                                                                                            $$ = (descType){.tc = $5, .tt = $2, .vb = 0, .vl = 0};
                                                                                          } 
                | MP_PROCEDURE identifier arguments MP_SCOLON                             { /* Install void function header */
                                                                                            genRoutineDec(UNDEFINED, identifierAtIndex($2), $3);

                                                                                            /* Install entry: Vector-Count (vc) stores arg count */
                                                                                            installIdEntry($2, TC_ROUTINE, UNDEFINED, 0, $3.length);

                                                                                            /* Increment table scope before installing arguments */
                                                                                            incrementTableScope();

                                                                                            /* Install arguments */
                                                                                            for (int i = 0; i < $3.length; i++) {
                                                                                              dataType *d = $3.list[i];
                                                                                              
                                                                                              /* Install each entry into the symbol table */
                                                                                              installIdEntry(d->id, d->tc, d->tt, d->vb, d->vl);
                                                                                            }

                                                                                            /* Free allocated memory */
                                                                                            freeDataList($3);

                                                                                            /* Custom DescType (.tc = Return Token-Type, .tt = Routine Name) */
                                                                                            $$ = (descType){.tc = UNDEFINED, .tt = $2, .vb = 0, .vl = 0};
                                                                                          }                                  
                ;

arguments : MP_POPEN parameterList MP_PCLOSE                                        { $$ = $2; }                      
          |                                                                         { $$ = initDataListType(); }
          ;

parameterList : identifierList MP_COLON type                                        { $$ = mapDescToDataList($3, $1); }                                              
              | parameterList MP_SCOLON identifierList MP_COLON type                { $$ = appendDataList(mapDescToDataList($5, $3), $1); }  
              ;

compoundStatement : MP_BEGIN optionalStatements MP_END    
                  ;

optionalStatements  : statementList                                                             
                    |                                                                                           
                    ;

statementList : statement                                                         
              | statementList MP_SCOLON statement                 
              ;

statement : variable MP_ASSIGNOP expression                       { /* Generate appropriate assignment by checking dataType token-class */
                                                                    if ($1->tc == TC_SCALAR) {
                                                                      genScalarAssignment(identifierAtIndex($1->id), $3->tn);
                                                                    } else {
                                                                      /* Extract IdEntry to obtain lower-bound information. */
                                                                      IdEntry *entry = containsIdEntry($1->id, $1->tc, SYMTAB_SCOPE_ALL);
                                                                      genVectorAssignment(identifierAtIndex($1->id), $1->ti, entry->vb, $3->tn);
                                                                    }
                                                                    freeDataType($1);
                                                                  }                 
          | procedureStatement
          | compoundStatement
          | MP_IF expression  { genIf($2->tn, $2->op); $<num>$ = getLbl(); genGoto($<num>$); reserveLbl(2); }
            MP_THEN statement { genGoto($<num>3 + 1); } 
            MP_ELSE           { genLblAt($<num>3); } statement { genLblAt($<num>3 + 1); freeDataType($2); }

          | MP_WHILE          { $<num>$ = genLbl(); reserveLbl(1); }
            expression        { genIf($3->tn, $3->op); genGoto($<num>2 + 1); }  
            MP_DO statement   { genGoto($<num>2); genLblAt($<num>2 + 1); freeDataType($3); }             
          ;

variable  : identifier                                            { /* Initialize dataType with identifier */
                                                                    $$ = initVarDataType(TC_SCALAR, $1);
                                                                  }                                                                                     
          | identifier MP_BOPEN expression MP_BCLOSE              { /* Initialize dataType with T-Label of expression indexing the vector. */
                                                                    $$ = initVarIdxDataType(TC_VECTOR, $1, $3->tn); freeDataType($3);
                                                                  }
                                                                                   
procedureStatement  : identifier                                 
                    | identifier MP_POPEN expressionList MP_PCLOSE  { /* Procedure call */

                                                                      /* Extract routine IdEntry */

                                                                      /* Generate routine call */
                                                                      genProcCall(identifierAtIndex($1), $3);

                                                                      /* Free allocated memory */
                                                                      freeDataList($3);
                                                                    }
                    | MP_READLN MP_POPEN expressionList MP_PCLOSE   { /* Generate corresponding readln function in C. */
                                                                      genReadLn($3);
                                                                      freeDataList($3); 
                                                                    }
                    | MP_WRITELN MP_POPEN expressionList MP_PCLOSE  { /* Generate corresponding writeln function in C. */
                                                                      genWriteLn($3);
                                                                      freeDataList($3); 
                                                                    }
                    ;

expressionList  : expression                                      { $$ = insertDataType($1, initDataListType()); }                                                              
                | expressionList MP_COMMA expression              { $$ = insertDataType($3, $1); }                
                ;

expression  : simpleExpression MP_RELOP_LT simpleExpression       { /* The boolean operator is saved for proper if-else conditional generation later */
                                                                    $$ = initExprCompDataType(genBoolOp( $1->tn, $3->tn), MP_RELOP_LT); freeDataType($1); freeDataType($3); 
                                                                  }
            | simpleExpression MP_RELOP_LE simpleExpression       { $$ = initExprCompDataType(genBoolOp( $1->tn, $3->tn), MP_RELOP_LE); freeDataType($1); freeDataType($3); }
            | simpleExpression MP_RELOP_EQ simpleExpression       { $$ = initExprCompDataType(genBoolOp( $1->tn, $3->tn), MP_RELOP_EQ); freeDataType($1); freeDataType($3); }
            | simpleExpression MP_RELOP_GE simpleExpression       { $$ = initExprCompDataType(genBoolOp( $1->tn, $3->tn), MP_RELOP_GE); freeDataType($1); freeDataType($3); }
            | simpleExpression MP_RELOP_GT simpleExpression       { $$ = initExprCompDataType(genBoolOp( $1->tn, $3->tn), MP_RELOP_GT); freeDataType($1); freeDataType($3); }
            | simpleExpression MP_RELOP_NE simpleExpression       { $$ = initExprCompDataType(genBoolOp( $1->tn, $3->tn), MP_RELOP_NE); freeDataType($1); freeDataType($3); }
            | simpleExpression                                    { $$ = $1; }
            ;

simpleExpression  : term                                          { $$ = $1; }
                  | sign term                                     { $$ = initExprConstDataType(genUnaryOp($2->tt, $1, $2->tn), $2->tt); freeDataType($2); }
                  | simpleExpression sign term                    { /* Type promote arithmetic result. Return new label with result of operation. Free allocated non-terminal. */
                                                                    unsigned tt = MAX($1->tt, $3->tt); $$ = initExprConstDataType(genArithOp(tt, $2, $1->tn, $3->tn), tt); freeDataType($1); freeDataType($3); 
                                                                  }
                  ;

term  : factor                                                    { $$ = $1; }
      | term MP_MULOP factor                                      { /* Type promote arithmetic result. Return new label with result of operation. Then free allocated non-terminals. */
                                                                    unsigned tt = MAX($1->tt, $3->tt); $$ = initExprConstDataType(genArithOp(tt, MP_MULOP, $1->tn, $3->tn), tt); freeDataType($1); freeDataType($3); }
      | term MP_DIVOP factor                                      { unsigned tt = MAX($1->tt, $3->tt); $$ = initExprConstDataType(genArithOp(tt, MP_DIVOP, $1->tn, $3->tn), tt); freeDataType($1); freeDataType($3); }
      | term MP_MODOP factor                                      { /* Type promotion is illogical for modulo. Result is always assumed to be integer. */ 
                                                                    $$ = initExprConstDataType(genArithOp(TT_INTEGER, MP_MODOP, $1->tn, $3->tn), TT_INTEGER); freeDataType($1); freeDataType($3); 
                                                                  }
      ;

factor  : identifier                                              {
                                                                    /* Extracting Entry and populating dataType fields. */
                                                                    IdEntry *entry = containsIdEntry($1, TC_ANY, SYMTAB_SCOPE_ALL);
                                                                    $$ = initExprVarDataType(genId(entry->tc, entry->tt, identifierAtIndex($1)), entry->tc, entry->tt, $1);
                                                                  }
        | identifier MP_POPEN expressionList MP_PCLOSE            { /* Function call */

                                                                    /* Extract routine IdEntry */
                                                                    IdEntry *entry = containsIdEntry($1, TC_ROUTINE, SYMTAB_SCOPE_ALL);

                                                                    /* Generate routine call */
                                                                    $$ = initExprConstDataType(genFuncCall(entry->tt, identifierAtIndex($1), $3), entry->tt);

                                                                    /* Free allocated memory */
                                                                    freeDataList($3);
                                                                  }
        | identifier MP_BOPEN expression MP_BCLOSE                {
                                                                    /* Extracting Entry. Generating indexed variable code */
                                                                    IdEntry *entry = containsIdEntry($1, TC_VECTOR, SYMTAB_SCOPE_ALL);
                                                                    $$ = initExprConstDataType(genVecIdx(entry->tt, identifierAtIndex($1), $3->tn, entry->vb), entry->tt); freeDataType($3); 
                                                                  } 
        | MP_INTEGER                                              { $$ = initExprConstDataType(genConst(TT_INTEGER, atof(yytext)), TT_INTEGER); }
        | MP_REAL                                                 { $$ = initExprConstDataType(genConst(TT_REAL, atof(yytext)), TT_REAL); }
        | MP_POPEN expression MP_PCLOSE                           { $$ = $2; }
        ;

identifier : MP_ID                                                { /* Dedicated rule is necessary to properly install token lexemes */
                                                                    $$ = installId(yytext); 
                                                                  }

sign  : MP_ADDOP                                                  { $$ = MP_ADDOP; }
      | MP_SUBOP                                                  { $$ = MP_SUBOP; }
      ;

%%

/*
 ********************************************************************************
 *                                  C Routines
 ********************************************************************************
*/

int main(int argc, char *argv[]) {

  // Verify argument count. 
  if (argc != 2) {
    fprintf(stderr, "./a.out <OutputFile>\n");
    exit(EXIT_FAILURE);
  }

  // Initialize supporting tables.
  initStringTable();
  initNumberTable();

  // Initialize IR code file.
  if (openIRFile(argv[1])) {
    fprintf(stderr, "Error: Couldn't open file!\n");
    exit(EXIT_FAILURE);
  }

  // Perform Intermediate-Code Generation.
  yyparse();

  // Free allocate memory.
  freeNumberTable();
  freeStringTable();
  freeSymbolTables();

  // Close file.
  closeIRFile();

  // Free Flex memory.
  yylex_destroy();
  return EXIT_SUCCESS;
}

