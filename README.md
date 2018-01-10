# MiniPascal

***

MiniPascal is a compiler for a largely simplified version of Pascal. It is fundamentally a toy for learning the workings of compilers, and shouldn't be taken seriously. 

## Design 

Variables may share identifiers so long as they are of a unique token-class. The possible token-classes are as follows:
1. Scalar.
2. Vector.
3. Routine.

Only Scalar and Vector class variables may be assigned to. Variables with the Routine class may not be assigned. 
Routines are defined as either Functions or Procedure. A Procedure is simply a function with no return value. MiniPascal
treats Procedures as Functions with undefined return types.

## Semantic Checks

### Expressions
1. Literal expressions are reduced/folded where possible.
2. Division-by-zero errors are thrown for division by a constant zero (may be derived).
4. Cascading errors are displayed for arithmetic operations involving undefined types.
5. Boolean expressions are treated as integer arithmetic expressions where zero and nonzero define false and true respectively.
6. Variable factors are checked for initialization.

### Assignments
1. Variable type-classes are checked to ensure they may be assigned to.
2. Assigned expressions are checked to ensure they are defined.
3. A warning is displayed for assignments which truncate the assigned expression.

### Routines
1. Checks if routine arguments are initialized.
2. Checks that correct number of arguments are supplied.
3. Checks that correct argument types are provided.
4. A warning is displayed for argument expressions which will be truncated.

### Control Flow
1. Guard expressions in conditional statements are checked for proper integer type.


### Compound Statements
1. A warning is displayed for empty compound statements.
2. An error is displayed if function return variables are left uninitialized.

###