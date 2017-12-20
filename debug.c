#include "debug.h"


void printStructure () {
    fprintf(stderr, C_TAF(BOL, BLK, "%s"), yytext);
}

void printControl() {
    fprintf(stderr, C_TAF(BOL, RED, "%s"), yytext);
}

void printIdentifier () {
    fprintf(stderr, C_TAF(BOL, BLU, "%s"), yytext);
}

void printLiteral () {
    fprintf(stderr, C_TAF(DIM, CYN, "%s"), yytext);
}

void printOperation () {
    fprintf(stderr, C_TAF(BOL, MAG, "%s"), yytext);
}

void printWhitespace () {
    switch (*yytext) {
        case '\n':
            fprintf(stderr, C_TAF(DIM, BLK, "\\n\n%d.\t"), lineNumber + 1);
            break;
        case '\t':
            fprintf(stderr, C_TAF(DIM, BLK, "--->"));
            break;
        default:
            fprintf(stderr, " ");
            break;
    }
}

void printToken (SyntaxType t) {
    static int init = 0;
    if (!init) {
        fprintf(stderr, C_TAF(DIM, BLK, "%d.\t"), lineNumber);
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
    }
}