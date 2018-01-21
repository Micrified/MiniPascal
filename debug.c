#include "debug.h"

/*
***************************************************************************
*                Private Type Definitions & Global Variables
***************************************************************************
*/

#define MAX_LINE_DEBUG  1000

// No-Warnings Flag.
int noWarnings;

// Line buffer for symantic debug purposes.
char lineBuffer[MAX_LINE_DEBUG];

// Line buffer lead pointer.
int lp;

/*
***************************************************************************
*                     Private Syntactic Debug Routines
***************************************************************************
*/

// Print routine for structural syntax.
static void printStructure () {
    lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(BOL, BLK, "%s"), yytext);
}

// Print routine for control characters.
static void printControl() {
    lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(BOL, BLK, "%s"), yytext);
}

// Print routine for identifiers.
static void printIdentifier () {
    lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(BOL, BLU, "%s"), yytext);
}

// Print routine for literals.
static void printLiteral () {
    lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(DIM, CYN, "%s"), yytext);
}

// Print routine for operators.
static void printOperation () {
    lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(BOL, MAG, "%s"), yytext);
}

// Print routine for whitespace.
static void printWhitespace () {
    switch (*yytext) {
        case '\n':
            if (inDebug) { fprintf(stderr, "%s" C_TAF(DIM, BLK, "\\n") "\n", lineBuffer); } 
            lp = 0;
            lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(DIM, BLK, "%d.\t"), yylineno + 1);
            break;
        case '\t':
            lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(DIM, BLK, "--->"));
            break;
        default:
            lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, " ");
            break;
    }
}

// Print routine for misplaced tokens.
static void printNaughty() {
    fprintf(stderr, C_TAF(BOL, RED, "\nBAD TOKEN: "));
    fprintf(stderr, C_TAF(UND, MAG, "%s"), yytext);
    putc('\n', stderr);
}

/*
***************************************************************************
*                          Syntactic Debug Routines
***************************************************************************
*/

void printToken (SyntaxType t) {
    static int init = 0;

    if (!init) {
        lp += snprintf(lineBuffer + lp, MAX_LINE_DEBUG - lp, C_TAF(DIM, BLK, "%d.\t"), yylineno);
        init = 1;
    }

    switch (t) {
        case Structure: printStructure();
        break;

        case Control:  printControl();
        break;

        case Literal: printLiteral();
        break;

        case Identifier: printIdentifier();
        break;
        
        case Operation: printOperation();
        break;

        case Whitespace: printWhitespace();
        break;

        case Naughty: printNaughty();
        break;

        case EndOfFile: if (inDebug) { fprintf(stderr, "%s\n", lineBuffer); };
    }
}

/*
***************************************************************************
*                          Semantic Debug Routines
***************************************************************************
*/

/* Prints a warning to stderr with description `msg`.
 * Accepts: Strings (%s), Integers (%d), and floats (%f) as arguments */
void printWarning (char *msg, ...) {
    char *p, *sval;
    int ival;
    double fval;

    // If in quiet mode: Do not print the warning.
    if (inQuiet) { 
        return;
    }

    // Initialize the variadic argument list.
    va_list ap;
    va_start(ap, msg);

    // Prints warning message header.
    fprintf(stderr, "\n" C_TAF(BOL, YEL, "Warning") " :: ");

    // Print formatted message: Set formatting.
    fprintf(stderr, CONFIG_AF(UND, YEL));

    // Print formatted message.
    for (p = msg; *p != '\0'; p++) {

        if (*p != '%') {
            putc(*p, stderr);
            continue;
        }

        switch (*(++p)) {
            case 'd':
                ival = va_arg(ap, int);
                fprintf(stderr, "%d", ival);
                break;
            case 'f':
                fval = va_arg(ap, double);
                fprintf(stderr, "%.3f", fval);
                break;
            case 's':
                for (sval = va_arg(ap, char *); *sval != '\0'; sval++) {
                    putc(*sval, stderr);
                }
                break;
            default:
                putchar(*p);
                break;
        }
    }
    va_end(ap);

    // End formatting.
    fprintf(stderr, RESET);

    // Print line.
    fprintf(stderr, "\n" C_TAF(BOL, RED, "--> ") "%s\n", lineBuffer);
}

/* Prints a error to stderr with description `msg`.
 * Accepts: Strings (%s), Integers (%d), and floats (%f) as arguments */
void printError (char *msg, ...) {
    char *p, *sval;
    int ival;
    double fval;

    // Initialize the variadic argument list.
    va_list ap;
    va_start(ap, msg);

    // Prints warning message header.
    fprintf(stderr, "\n" C_TAF(BOL, RED, "Error") " :: ");

    // Print formatted message: Set formatting.
    fprintf(stderr, CONFIG_AF(UND, RED));

    for (p = msg; *p != '\0'; p++) {

        if (*p != '%') {
            putc(*p, stderr);
            continue;
        }

        switch (*(++p)) {
            case 'd':
                ival = va_arg(ap, int);
                fprintf(stderr, "%d", ival);
                break;
            case 'f':
                fval = va_arg(ap, double);
                fprintf(stderr, "%.3f", fval);
                break;
            case 's':
                for (sval = va_arg(ap, char *); *sval != '\0'; sval++) {
                    putc(*sval, stderr);
                }
                break;
            default:
                putchar(*p);
                break;
        }
    }
    va_end(ap);

    // End formatting.
    fprintf(stderr, RESET);

    // Print line.
    fprintf(stderr, "\n" C_TAF(BOL, RED, "--> ") "%s\n", lineBuffer);
}
