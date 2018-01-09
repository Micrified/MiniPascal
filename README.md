# MiniPascal

***

MiniPascal is a compiler for a largely simplified version of Pascal. It is fundamentally a toy for learning the workings of compilers, and shouldn't be taken seriously. 

## Checks

### Expressions
1. Expressions with literal constants are reduced.
2. Expressions are checked for division by zero (when divisor is constant).
3. Warnings are given for expression involving mismatching primitive types.
4. Errors are given for expressions involving illegal types.
5. Boolean expressions are treated as arithmetic expressions with (1/0) constant values.

**Examples**

Description: Division by zero.
Input: `x := y / (10 MOD (5 + 5));`
Output: "Error :: Division by zero!"

Description: Assignment of REAL to INTEGER.
Input: `VAR x : INTEGER; ... x := 5 + 3.14;`
Output: "Warning :: Assigned value will be truncated!"


Input: `x := 5 + 3.14;`
Output: "Error :: "x" is undefined!", "Error :: "x" of type "Undefined" is not assignable!"


### Assignments

### Functions

### Procedures

### Control Flow

### Compound Statements

###