#include "irgen.h"

/*
***************************************************************************
*               Internal Symbolic Constants & Global Variables
***************************************************************************
*/

/* Counter for T-labels (temporary labels for expression operands) */
static unsigned t;

/* Counter for L-labels (labels for control-flow like loops) */
static unsigned l;

/*
***************************************************************************
*                  Internal Generation Routines
***************************************************************************
*/

/* Returns a corresponding C type for a given MP_<token> or TT_<token> */
static const char *getCType (unsigned tt) {
    switch (tt) {
        case UNDEFINED:     return "void";
        case TT_INTEGER:    return "int";
        case MP_INTEGER:    return "int";
        case TT_REAL:       return "double";
        case MP_REAL:       return "double";
    }
    fprintf(stderr, "Error: getCType: Unknown token-type %u!\n", tt);
    exit(EXIT_FAILURE);
}

/* Returns a corresponding C operator for a given MP_<token> */
static const char *getCOp (unsigned operator) {
    switch (operator) {

        // Arithmetic Operators.
        case MP_ADDOP:      return "+";
        case MP_SUBOP:      return "-";
        case MP_MULOP:      return "*";
        case MP_DIVOP:      return "/";
        case MP_MODOP:      return "%";

        // Boolean Operators.
        case MP_RELOP_LT:   return "<";
        case MP_RELOP_LE:   return "<=";
        case MP_RELOP_EQ:   return "==";
        case MP_RELOP_GE:   return ">=";
        case MP_RELOP_GT:   return ">";
        case MP_RELOP_NE:   return "!=";
    }
    fprintf(stderr, "Error: getCOp: Unknown operator %u!\n", operator);
    exit(EXIT_FAILURE);
}

/* Returns a inverted C operator for the given MP_<token> operator */
static const char *invOp (unsigned operator) {
    switch (operator) {
        case MP_RELOP_LT:   return ">=";
        case MP_RELOP_LE:   return ">";
        case MP_RELOP_EQ:   return "!=";
        case MP_RELOP_GE:   return "<";
        case MP_RELOP_GT:   return "<=";
        case MP_RELOP_NE:   return "==";
    }
    fprintf(stderr, "Error: invOp: Unknown operator %u!\n", operator);
    exit(EXIT_FAILURE);
}

/* Writes a comma delimited list of C-type datatypes to given FILE */
static void writeDataList (FILE *fp, dataListType dataList, unsigned onlyId) {
    for (int i = 0; i < dataList.length; i++) {
        dataType *dt = dataList.list[i];
        const char *p = (dt->tc == TC_VECTOR) ? "[]" : "";
        if (onlyId) {
            if (dt->id == NIL) {
                fprintf(fp, "t%u", dt->tn);
            } else {
                fprintf(fp, "%s", identifierAtIndex(dt->id));
            }
        } else {
            fprintf(fp, "%s %s%s", getCType(dt->tt), identifierAtIndex(dt->id), p);
        }
        if (i < dataList.length - 1) {
            fprintf(fp, ", ");
        }
    }
}

/*
***************************************************************************
*                     Expression Generation Routines
***************************************************************************
*/

/* Generates a T-Label for given constant of type `tt`. Returns T-Label num */
unsigned genConst (unsigned tt, double n) {
    if (tt == TT_REAL) {
        fprintf(irfp, "double t%u = %f;\n", t, n);
    } else {
        fprintf(irfp, "int t%u = %d;\n", t, (int)n);
    }
    return t++;
}

/* Generates a T-Label for given identifier. Returns T-Label num */
unsigned genId (unsigned tc, unsigned tt, const char *identifier) {
    if (tc == TC_SCALAR) {
        fprintf(irfp, "%s t%u = %s;\n", getCType(tt), t, identifier);
    } else {
        fprintf(irfp, "%s *t%u = %s;\n", getCType(tt), t, identifier);
    }
    return t++;
}

/* Generates a T-Label for an T-indexed vector. Returns T-Label num. */
unsigned genVecIdx (unsigned tt, const char *identifier, unsigned ti, unsigned vb) {
    unsigned adjustedTi = t;
    fprintf(irfp, "int t%u = t%u - %u;\n", t++, ti, vb);
    fprintf(irfp, "%s t%u = %s[t%u];\n", getCType(tt), t, identifier, adjustedTi);
    return t++;
}

/* Generates a T-Label for a unary operation (-|+) ti. Returns T-Label num. */
unsigned genUnaryOp (unsigned tt, unsigned operator, unsigned ti) {
    fprintf(irfp, "%s t%u = %st%u;\n", getCType(tt), t, getCOp(operator), ti);
    return t++;
}

/* Generates a T-Label for arithmetic operation. (tx op ty). Returns T-Label num. */
unsigned genArithOp (unsigned tt, unsigned operator, unsigned tx, unsigned ty) {
    fprintf(irfp, "%s t%u = t%u %s t%u;\n", getCType(tt), t, tx, getCOp(operator), ty);
    return t++;
}

/* Generates a T-Label for a boolean operation (tx - ty). Returns T-Label num. */
unsigned genBoolOp (unsigned tx, unsigned ty) {
    fprintf(irfp, "int t%u = t%u - t%u;\n", t, tx, ty);
    return t++;
}

/* Generates a T-Label for a function invocation. Returns T-Label num. */
unsigned genFuncCall (unsigned tt, const char *name, dataListType args) {

    // Generate call head.
    fprintf(irfp, "%s t%u = %s", getCType(tt), t, name);

    // Generate args.
    putc('(', irfp);
    writeDataList(irfp, args, 1);
    putc(')', irfp);

    // Generate call end.
    fprintf(irfp, ";\n");

    return t++;
}

/* Generates a T-Label for a procedure invocation. No label returned. */
void genProcCall (const char *name, dataListType args) {
    
    // Generate call head.
    fprintf(irfp, "%s", name);

    // Generate args.
    putc('(', irfp);
    writeDataList(irfp, args, 1);
    putc(')', irfp);

    // Generate call end.
    fprintf(irfp, ";\n");
}


/*
***************************************************************************
*                     Assignment Generation Routines
***************************************************************************
*/

/* Generates a T-Label for a scalar assignment. */
void genScalarAssignment (const char *identifier, unsigned ti) {
    fprintf(irfp, "%s = t%u;\n", identifier, ti);
}

/* Generates a T-Label for a vector-index assignment. */
void genVectorAssignment (const char *identifier, unsigned ti, unsigned vb, unsigned te) {
    unsigned adjustedTi = t;
    fprintf(irfp, "int t%u = t%u - %u;\n", t++, ti, vb);
    fprintf(irfp, "%s[t%u] = t%u;\n", identifier, adjustedTi, te);
}

/*
***************************************************************************
*                    Control-Flow Generation Routines
***************************************************************************
*/

/* Returns the current label */
unsigned getLbl() {
    return l;
}

/* Generates a L-Label. Returns L-Label num. */
unsigned genLbl() {
    fprintf(irfp, "Lab%u: ;\n", l);
    return l++;
}

/* Generates a L-Label at given number. Returns L-Label num. */
unsigned genLblAt (unsigned l) {
    fprintf(irfp, "Lab%u: ;\n", l);
    return l;
}

/* Reserves the next n L-Labels. */
void reserveLbl (unsigned n) {
    l += n;
}

/* Generates a GOTO conditional statement with destination L-Label */
void genGoto (unsigned l) {
    fprintf(irfp, "goto Lab%u;\n", l);
}

/* Generates a conditional statement (inverts op). Does not print newline. */
void genIf (unsigned ti, unsigned op) {
    fprintf(irfp, "if (t%u %s 0) ", ti, invOp(op));
}

/*
***************************************************************************
*                    Declaration Generation Routines
***************************************************************************
*/

/* Generates a scalar declaration. */
void genScalarDec (unsigned tt, const char *identifier) {
    fprintf(irfp, "%s %s;\n", getCType(tt), identifier);
}

/* Generates a vector decalaration. */
void genVectorDec (unsigned tt, unsigned n, const char *identifier) {
    fprintf(irfp, "%s %s[%d];\n", getCType(tt), identifier, n);
}

/*
***************************************************************************
*                       Routine Generation Routines
***************************************************************************
*/

/* Generates a routine declaration */
void genRoutineDec (unsigned tt, const char *name, dataListType args) {

    // Install return type and name.
    fprintf(irfp, "%s %s", getCType(tt), name);

    // Install arguments */
    putc('(', irfp);
    writeDataList(irfp, args, 0);
    putc(')', irfp);
}

/* Generates a routine return statement */
void genReturn (unsigned tt, unsigned id) {
    if (tt == UNDEFINED) {
        fprintf(irfp, "return;\n");
    } else {
        fprintf(irfp, "return %s;\n", identifierAtIndex(id));
    }
}

/*
***************************************************************************
*                    Structural Generation Routines
***************************************************************************
*/

/* Generates the main program header and opening brace */
void genMainHeader () {
    fprintf(irfp, "int main () {\n");
}

/* Generates the return statement and closing brace for main */
void genMainEnd () {
    fprintf(irfp, "return 0;\n}\n");
}

/* Generates the opening of a block (newline + Opening brace + newline) */
void genBlockStart() {
    fprintf(irfp, "\n{\n");
}

/* Generates the end of a block (newline + Closing brace + newline) */
void genBlockEnd() {
    fprintf(irfp, "}\n");
}

/*
***************************************************************************
*                 Library Procedure Generation Routines
***************************************************************************
*/

/* Generates a statement to scan in values. */
void genReadLn (dataListType dataList) {
    fprintf(irfp, "scanf(\"");

    // Generate format string.
    for (int i = 0; i < dataList.length; i++) {
        dataType *dt = dataList.list[i];
        fprintf(irfp, "%s", (dt->tt == TT_INTEGER) ? "%d" : "%f");
    }
    fprintf(irfp, "\",");

    // Generate argument list.
    for (int i = 0; i < dataList.length; i++) {
       dataType *dt = dataList.list[i];
       fprintf(irfp, "&%s", identifierAtIndex(dt->id));
       if (i < dataList.length - 1) {
           fprintf(irfp, ",");
       } 
    }

    fprintf(irfp, ");\n");
}

/* Generates a statement to print values. */
void genWriteLn (dataListType dataList) {
    fprintf(irfp, "printf(\"");

    // Generate format string.
    for (int i = 0; i < dataList.length; i++) {
        dataType *dt = dataList.list[i];
        fprintf(irfp, "%s ", (dt->tt == TT_INTEGER) ? "%d" : "%f");
    }
    fprintf(irfp, "\\n\",");

    // Generate argument list.
    for (int i = 0; i < dataList.length; i++) {
       dataType *dt = dataList.list[i];
       fprintf(irfp, "%s", identifierAtIndex(dt->id));
       if (i < dataList.length - 1) {
           fprintf(irfp, ",");
       } 
    }

    fprintf(irfp, ");\n");
}