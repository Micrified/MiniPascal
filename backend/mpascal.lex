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

%}

ws              [ \t]
digit           [0-9]
letter          [a-zA-Z]
identifier      {letter}({letter}|{digit}|"_")*
integer         {digit}+
real            {integer}("."{integer})?
mulop           [*]
comment         \{[^\}]*\}

%%

(?i:READLN)     { return MP_READLN;      }
(?i:WRITELN)    { return MP_WRITELN;     }

(?i:WHILE)      { return MP_WHILE;       }
(?i:DO)         { return MP_DO;          }

(?i:IF)         { return MP_IF;          }
(?i:THEN)       { return MP_THEN;        }
(?i:ELSE)       { return MP_ELSE;        }

(?i:BEGIN)      { return MP_BEGIN;       }
(?i:END)        { return MP_END;         }

(?i:FUNCTION)   { return MP_FUNCTION;    }
(?i:PROCEDURE)  { return MP_PROCEDURE;   }
(?i:ARRAY)      { return MP_ARRAY;       }
(?i:OF)         { return MP_OF;          }
(?i:VAR)        { return MP_VAR;         }
(?i:PROGRAM)    { return MP_PROGRAM;     }

(?i:INTEGER)    { return MP_TYPE_INTEGER;}
(?i:REAL)       { return MP_TYPE_REAL;   }

{integer}       { return MP_INTEGER;      }
{real}          { return MP_REAL;         }

":="            { return MP_ASSIGNOP;  }

"<"             { return MP_RELOP_LT; }
"<="            { return MP_RELOP_LE; }
"="             { return MP_RELOP_EQ; }
">="            { return MP_RELOP_GE; }
">"             { return MP_RELOP_GT; }
"<>"            { return MP_RELOP_NE; }

"+"             { return MP_ADDOP;    }
"-"             { return MP_SUBOP;    }

{mulop}         { return MP_MULOP;    } 
(?i:DIV)        { return MP_DIVOP;    }
(?i:MOD)        { return MP_MODOP;    }
"/"             { return MP_DIVOP;    }

","             { return MP_COMMA;    }
"("             { return MP_POPEN;    }                 
")"             { return MP_PCLOSE;   }
"["             { return MP_BOPEN;    }
"]"             { return MP_BCLOSE;   }
":"             { return MP_COLON;    }
";"             { return MP_SCOLON;   }
".."            { return MP_ELLIPSES; }
"."             { return MP_FSTOP;    }
\n              { yylineno++;         }
{ws}            {                        }
{identifier}    { return MP_ID;       }
{comment}       {             }

<<EOF>>         { return MP_EOF;      }
.               { return MP_WTF;      }

%%
