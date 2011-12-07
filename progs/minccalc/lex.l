%{
#include <string.h>
#include "node.h"
#include "gram.h"

int lexpos = 0;
#define setpos() yylval.pos = lexpos; lexpos += yyleng
%}

%%

avg            setpos(); return AVG;
sum            setpos(); return SUM;
prod           setpos(); return PROD;
let            setpos(); return LET;
len            setpos(); return LEN;
isnan          setpos(); return ISNAN;
sqrt           setpos(); return SQRT;
abs            setpos(); return ABS;
max            setpos(); return MAX;
min            setpos(); return MIN;
imax           setpos(); return IMAX;
imin           setpos(); return IMIN;
exp            setpos(); return EXP;
log            setpos(); return LOG;
sin            setpos(); return SIN;
cos            setpos(); return COS;
tan            setpos(); return TAN;
asin           setpos(); return ASIN;
acos           setpos(); return ACOS;
atan           setpos(); return ATAN;
clamp          setpos(); return CLAMP;
segment        setpos(); return SEGMENT;
in             setpos(); return IN;
to             setpos(); return TO;
if             setpos(); return IF;
else           setpos(); return ELSE;
for            setpos(); return FOR;
NaN            setpos(); return NAN;
\<             setpos(); return LT;
\<=            setpos(); return LE;
\>             setpos(); return GT;
\>=            setpos(); return GE;
==             setpos(); return EQ;
\!=            setpos(); return NE;
\!             setpos(); return NOT;
&&             setpos(); return AND;
\|\|           setpos(); return OR;
[a-zA-Z][_a-zA-Z0-9]*          {
                  setpos();
                  yylval.ident = new_ident(yytext); 
                  return IDENT; 
               }
[0-9]+(\.[0-9]*)?([eE][-+]?[0-9]+)? {
                  setpos();
                  yylval.real = atof(yytext);;
                  return REAL;
               }
[ \t\n\f]+            setpos();
.              setpos(); return yytext[0];


%%

static YY_BUFFER_STATE lex_state;

void lex_init(const char *s){
   lex_state = yy_scan_string(s);
}

void lex_finalize(){
   yy_delete_buffer(lex_state);
}

int yywrap(){
   return 1;
}
