%{
    #include <stdlib.h>
    #include "mpascal.tab.h"

    /*
     ***************************************************************************
     *                      Mini Pascal Lexical Analyser                       *
     * AUTHORS: Charles Randolph, Joe Jones.                                   *
     * SNUMBERS: s2897318, s2990652.                                           *
     ***************************************************************************
    */

    /* Debugging toolset */
    #include "debug.c"

    /* Counts newlines in the given string. Used to count newlines in comments */
    int newlineCount (const char *sp);
%}

ws              [ \t]
digit           [0-9]
letter          [a-zA-Z]
identifier      {letter}({letter}|{digit}|"_")*
integer         {digit}+
real            {integer}("."{integer})?
addop           [+-]
mulop           [*]
comment         \{[^\}]*\}

%%

(?i:WHILE)      { printToken(Control);   return MP_WHILE;       }
(?i:DO)         { printToken(Control);   return MP_DO;          }

(?i:IF)         { printToken(Control);   return MP_IF;          }
(?i:THEN)       { printToken(Control);   return MP_THEN;        }
(?i:ELSE)       { printToken(Control);   return MP_ELSE;        }

(?i:BEGIN)      { printToken(Control);   return MP_BEGIN;       }
(?i:END)        { printToken(Control);   return MP_END;         }

(?i:FUNCTION)   { printToken(Control);   return MP_FUNCTION;    }
(?i:PROCEDURE)  { printToken(Control);   return MP_PROCEDURE;   }
(?i:ARRAY)      { printToken(Control);   return MP_ARRAY;       }
(?i:OF)         { printToken(Control);   return MP_OF;          }
(?i:VAR)        { printToken(Control);   return MP_VAR;         }
(?i:PROGRAM)    { printToken(Control);   return MP_PROGRAM;     }

(?i:INTEGER)    { printToken(Control);   return MP_TYPE_INTEGER;}
(?i:REAL)       { printToken(Control);   return MP_TYPE_REAL;   }

{integer}       { printToken(Literal);  return MP_INTEGER;      }
{real}          { printToken(Literal);  return MP_REAL;         }

":="            { printToken(Operation);   return MP_ASSIGNOP;  }

"<"             { printToken(Operation);    return MP_RELOP;    }
"<="            { printToken(Operation);    return MP_RELOP;    }
"="             { printToken(Operation);    return MP_RELOP;    }
">="            { printToken(Operation);    return MP_RELOP;    }
">"             { printToken(Operation);    return MP_RELOP;    }
"<>"            { printToken(Operation);    return MP_RELOP;    }

{addop}         { printToken(Operation);    return MP_ADDOP;    }

{mulop}         { printToken(Operation);    return MP_MULOP;    } 
(?i:DIV)        { printToken(Operation);    return MP_DIVOP;    }
(?i:MOD)        { printToken(Operation);    return MP_MODOP;    }
"/"             { printToken(Operation);    return MP_DIVOP;    }

","             { printToken(Structure);    return MP_COMMA;    }
"("             { printToken(Structure);    return MP_POPEN;    }                 
")"             { printToken(Structure);    return MP_PCLOSE;   }
"["             { printToken(Structure);    return MP_BOPEN;    }
"]"             { printToken(Structure);    return MP_BCLOSE;   }
":"             { printToken(Structure);    return MP_COLON;    }
";"             { printToken(Structure);    return MP_SCOLON;   }
".."            { printToken(Structure);    return MP_ELLIPSES; }
"."             { printToken(Structure);    return MP_FSTOP;    }
\n              { printToken(Whitespace);   yylineno++;         }
{ws}            { printToken(Whitespace);                       }
{identifier}    { printToken(Identifier);   return MP_ID;       }
{comment}       { yylineno += newlineCount(yytext);             }

<<EOF>>         { printToken(Control);  return MP_EOF;          }
.               { printToken(Naughty);  return MP_WTF;          }

%%

/* Counts the number of newlines in the given string */
int newlineCount (const char *sp) {
    int c, count = 0;
    while ((c = *sp++) != '\0') {
        count += (c == '\n');
    }
    return count;
}