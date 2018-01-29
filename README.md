# MiniPascal

***

MiniPascal is a compiler for a largely simplified version of Pascal. It is fundamentally a toy for learning the workings of compilers, and shouldn't be taken seriously. 

### Compiling

Compiling takes only a couple of steps.

1. Build frontend and backend executables in top-level directory: `make subsystem` 
2. Build the top level program (mpc) with: `make`

That's it, the program should be ready to go. Simply invoke with `./mpc <inputfile> <outputfile>`

## Design 

Variables may share identifiers so long as they are of a unique token-class. The possible token-classes are as follows:
1. Scalar.
2. Vector.
3. Routine.

### Assignments
Only Scalar and Vector class variables may be assigned to. Variables with the Routine class may not be assigned.

### Routines 
Routines are defined as either Functions or Procedure. A Procedure is simply a function with no return value. MiniPascal
treats Procedures as Functions with undefined return types. Function return values are scalar class variables which share the same name as the function. They are installed automatically.

**Note:** Routines only accept **scalar** and **vector** arguments.

You are required to provide routines with all their defined arguments. There are two exceptions to this. 
* `readln(...)`
* `writeln(...)`

The above procedures accept a variable number of arguments. Neither `writeln` or `readln(...)` may be given constant or vector arguments, however.

## Semantic Checks

### Expressions
1. Constant expressions are reduced/folded where possible.
2. Division-by-zero errors are thrown when division by a constant expression which is equivalent to zero (may be derived).
4. Cascading errors are displayed for arithmetic operations involving undefined types.
5. Boolean expressions are treated as integer arithmetic expressions where zero and nonzero define false and true respectively.
7. Function calls and array indexes are reduced to their primitive types (Integer/Real).

### Assignments
1. Variable type-classes are checked to ensure they may be assigned to.
2. Assigned expressions are checked to ensure they are defined.
3. A warning is displayed for assignments which truncate the assigned expression.
4. An error is displayed for assignments declared above the maximum scope level. 
5. Variables being assigned another variable directly (i.e: x := y) will throw warnings if y is uninitialized.

### Routines
2. Checks that the number of arguments supplied are not equal to the defined amount (exceptions for `readln`,`writeln`).
3. Checks that correct argument types are provided.
4. A warning is displayed for argument expressions which will be truncated.

### Control Flow
1. Guard expressions in conditional statements are checked for proper integer type.


### Compound Statements
1. An error is displayed if function return variables are left uninitialized. This is scope specific.

###