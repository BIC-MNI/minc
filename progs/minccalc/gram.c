#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "gram.y"
#include <stdio.h>
#include <errno.h>
#include "node.h"
#line 7 "gram.y"
typedef union{
int		pos;
node_t 		node;
float		real;
ident_t		ident;
} YYSTYPE;
#line 23 "y.tab.c"
#define IN 257
#define TO 258
#define IDENT 259
#define REAL 260
#define AVG 261
#define SUM 262
#define LET 263
#define NEG 264
#define LEN 265
#define ISNAN 266
#define NOT 267
#define LT 268
#define LE 269
#define GT 270
#define GE 271
#define EQ 272
#define NE 273
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    2,
    2,    3,    3,
};
short yylen[] = {                                         2,
    1,    3,    3,    7,    5,    5,    5,    5,    3,    3,
    2,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    4,    2,    2,    2,    2,    2,    2,    1,    1,    5,
    5,    1,    3,
};
short yydefred[] = {                                      0,
   28,   29,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   22,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    2,    0,    0,    0,    3,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   21,    0,    0,    5,    6,    7,    8,
    0,    0,   30,    0,    4,
};
short yydgoto[] = {                                      13,
   14,   18,   27,
};
short yysindex[] = {                                    477,
    0,    0,  477,  477, -253,  477,  477,  477,  477,  477,
  477,  504,    0,  485,  -82,  -82,  -50,    0,  -82,  485,
  485,  -87,  129,  442, -245,  485,  -44,  477,  477,  477,
  477,  477,  477,  477,  477,  477,  477,  477,  477,  477,
    0,  477,  477,  477,    0,  477,   -6,   -6,   -6,   -6,
   -6,   -6,  -37,  -37,  -87,  -87,  448,  -87,  293,  146,
  409,  454,  485,    0,  477, -253,    0,    0,    0,    0,
  477,  485,    0,  462,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   13,    1,    8,    0,    0,   18,   73,
   75,   25,    0,    0,  427,  -42,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  120,  139,  175,  192,
  209,  301,   86,  103,   35,   93,    0,  110,    0,    0,
    0,    0,  -41,    0,    0,    0,    0,    0,    0,    0,
    0,  395,    0,    0,    0,
};
short yygindex[] = {                                      0,
  582,  -52,    0,
};
#define YYTABLESIZE 771
short yytable[] = {                                      46,
   25,   32,   33,   38,   36,   17,   39,   24,   38,   37,
   40,   44,    1,   73,    0,    0,    0,   26,    0,    0,
    0,    0,    0,    0,   11,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   12,   36,   34,    0,   35,    0,
   37,   25,   25,   25,   25,   25,    0,   25,   24,   24,
   24,   24,   24,   38,   24,    0,   39,    0,   26,   26,
   26,   26,   26,    0,   26,   11,   11,   11,   11,   11,
    0,   11,   27,    0,   23,   12,   12,   12,   12,   12,
   45,   12,   32,   33,   38,    9,    0,   39,    0,    0,
    0,    0,   13,   25,   25,    0,    0,    0,    0,    0,
   24,   24,   10,    0,    0,    0,    0,    0,    0,   14,
   26,   26,    0,   27,    0,   23,   27,   11,   23,   15,
    0,    0,    0,    0,   25,   25,    9,   12,    9,    9,
    9,   24,   24,   13,   13,   13,   13,   13,   16,   13,
    0,   26,   26,   10,    0,   10,   10,   10,   11,   11,
   14,   14,   14,   14,   14,    0,   14,    0,   12,   12,
   15,    0,    0,   15,    0,   27,    0,   23,    0,   41,
   36,   34,   42,   35,   17,   37,    0,    0,    9,   16,
    0,    0,   16,    0,    0,   13,   67,   36,   34,    0,
   35,   18,   37,    0,    0,   10,   27,   27,   23,   23,
    0,    0,   14,    0,    0,    0,    0,    0,   19,    9,
    9,    0,   15,    0,    0,   17,   13,   13,   17,   38,
    0,    0,   39,    0,    0,    0,   10,   10,    0,    0,
    0,   16,   18,   14,   14,   18,   38,    0,   68,   39,
    0,    0,    0,   15,   15,    0,    0,    0,    0,   19,
    0,    0,   19,    0,    0,    0,    0,   25,    0,    0,
    0,    0,   16,   16,   24,    0,    0,   17,   25,   25,
   25,   25,   25,   25,   26,   24,   24,   24,   24,   24,
   24,   11,    0,    0,   18,   26,   26,   26,   26,   26,
   26,   12,   11,   11,   11,   11,   11,   11,   17,   17,
   20,   19,   12,   12,   12,   12,   12,   12,    0,    0,
    0,    0,    0,    0,    0,   18,   18,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   27,
    0,   23,   19,   19,   36,   34,   66,   35,    0,   37,
    0,   20,    9,    0,   20,    0,    0,    0,    0,   13,
    0,    0,    0,    9,    9,    9,    9,    9,    9,   10,
   13,   13,   13,   13,   13,   13,   14,    0,    0,    0,
   10,   10,   10,   10,   10,   10,   15,   14,   14,   14,
   14,   14,   14,   38,    0,    0,   39,   15,   15,   15,
   15,   15,   15,   20,   31,   16,   28,   29,   30,   31,
   32,   33,    0,    0,    0,    0,   16,   16,   16,   16,
   16,   16,    0,   28,   29,   30,   31,   32,   33,    0,
    0,    0,    0,    0,   20,   20,    0,    0,    0,    0,
    0,   17,    0,    0,    0,   31,    0,    0,   31,    0,
    0,    0,   17,   17,   17,   17,   17,   17,   18,   69,
   36,   34,    0,   35,    0,   37,    0,    0,    0,   18,
   18,   18,   18,   18,   18,   19,    0,    0,   28,   28,
   28,   28,    0,   28,    0,    0,   19,   19,   19,   19,
   19,   19,    0,   36,   34,   43,   35,   31,   37,   36,
   34,    0,   35,    0,   37,   36,   34,    0,   35,   38,
   37,   70,   39,   36,   34,    0,   35,    0,   37,    0,
    0,    0,    0,    0,    0,    0,   10,   28,   31,   31,
   28,    9,    0,    0,    0,    0,   36,   34,    0,   35,
    0,   37,   38,    0,    0,   39,    0,    0,   38,    0,
   64,   39,    0,   10,   38,    0,    0,   39,    9,   65,
    0,   28,   38,    0,    0,   39,    0,   20,    0,    0,
   28,   29,   30,   31,   32,   33,    0,   11,   20,   20,
   20,   20,   20,   20,    0,   38,    0,   71,   39,    0,
    0,    0,    0,    0,   15,   16,   75,   19,   20,   21,
   22,   23,   24,   26,   11,    0,    0,    0,    0,   12,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,
   58,   59,    0,   60,   61,   62,   12,   63,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   72,    0,    0,    0,
    0,   31,   74,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   28,   29,   30,   31,
   32,   33,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   28,   28,   28,   28,   28,   28,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   28,
   29,   30,   31,   32,   33,   28,   29,   30,   31,   32,
   33,   28,   29,   30,   31,   32,   33,    0,    0,   28,
   29,   30,   31,   32,   33,    1,    2,    3,    4,    5,
    0,    6,    7,    8,    0,    0,    0,    0,    0,    0,
    0,    0,   28,   29,   30,   31,   32,   33,    0,    0,
    0,    0,   25,    2,    3,    4,    5,    0,    6,    7,
    8,
};
short yycheck[] = {                                      44,
    0,   44,   44,   91,   42,  259,   94,    0,   91,   47,
   61,  257,    0,   66,   -1,   -1,   -1,    0,   -1,   -1,
   -1,   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,    0,   42,   43,   -1,   45,   -1,
   47,   41,   42,   43,   44,   45,   -1,   47,   41,   42,
   43,   44,   45,   91,   47,   -1,   94,   -1,   41,   42,
   43,   44,   45,   -1,   47,   41,   42,   43,   44,   45,
   -1,   47,    0,   -1,    0,   41,   42,   43,   44,   45,
  125,   47,  125,  125,   91,    0,   -1,   94,   -1,   -1,
   -1,   -1,    0,   93,   94,   -1,   -1,   -1,   -1,   -1,
   93,   94,    0,   -1,   -1,   -1,   -1,   -1,   -1,    0,
   93,   94,   -1,   41,   -1,   41,   44,   93,   44,    0,
   -1,   -1,   -1,   -1,  124,  125,   41,   93,   43,   44,
   45,  124,  125,   41,   42,   43,   44,   45,    0,   47,
   -1,  124,  125,   41,   -1,   43,   44,   45,  124,  125,
   41,   42,   43,   44,   45,   -1,   47,   -1,  124,  125,
   41,   -1,   -1,   44,   -1,   93,   -1,   93,   -1,   41,
   42,   43,   44,   45,    0,   47,   -1,   -1,   93,   41,
   -1,   -1,   44,   -1,   -1,   93,   41,   42,   43,   -1,
   45,    0,   47,   -1,   -1,   93,  124,  125,  124,  125,
   -1,   -1,   93,   -1,   -1,   -1,   -1,   -1,    0,  124,
  125,   -1,   93,   -1,   -1,   41,  124,  125,   44,   91,
   -1,   -1,   94,   -1,   -1,   -1,  124,  125,   -1,   -1,
   -1,   93,   41,  124,  125,   44,   91,   -1,   93,   94,
   -1,   -1,   -1,  124,  125,   -1,   -1,   -1,   -1,   41,
   -1,   -1,   44,   -1,   -1,   -1,   -1,  257,   -1,   -1,
   -1,   -1,  124,  125,  257,   -1,   -1,   93,  268,  269,
  270,  271,  272,  273,  257,  268,  269,  270,  271,  272,
  273,  257,   -1,   -1,   93,  268,  269,  270,  271,  272,
  273,  257,  268,  269,  270,  271,  272,  273,  124,  125,
    0,   93,  268,  269,  270,  271,  272,  273,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  124,  125,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,
   -1,  257,  124,  125,   42,   43,   44,   45,   -1,   47,
   -1,   41,  257,   -1,   44,   -1,   -1,   -1,   -1,  257,
   -1,   -1,   -1,  268,  269,  270,  271,  272,  273,  257,
  268,  269,  270,  271,  272,  273,  257,   -1,   -1,   -1,
  268,  269,  270,  271,  272,  273,  257,  268,  269,  270,
  271,  272,  273,   91,   -1,   -1,   94,  268,  269,  270,
  271,  272,  273,   93,    0,  257,  268,  269,  270,  271,
  272,  273,   -1,   -1,   -1,   -1,  268,  269,  270,  271,
  272,  273,   -1,  268,  269,  270,  271,  272,  273,   -1,
   -1,   -1,   -1,   -1,  124,  125,   -1,   -1,   -1,   -1,
   -1,  257,   -1,   -1,   -1,   41,   -1,   -1,   44,   -1,
   -1,   -1,  268,  269,  270,  271,  272,  273,  257,   41,
   42,   43,   -1,   45,   -1,   47,   -1,   -1,   -1,  268,
  269,  270,  271,  272,  273,  257,   -1,   -1,   42,   43,
   44,   45,   -1,   47,   -1,   -1,  268,  269,  270,  271,
  272,  273,   -1,   42,   43,   44,   45,   93,   47,   42,
   43,   -1,   45,   -1,   47,   42,   43,   -1,   45,   91,
   47,   93,   94,   42,   43,   -1,   45,   -1,   47,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   40,   91,  124,  125,
   94,   45,   -1,   -1,   -1,   -1,   42,   43,   -1,   45,
   -1,   47,   91,   -1,   -1,   94,   -1,   -1,   91,   -1,
   93,   94,   -1,   40,   91,   -1,   -1,   94,   45,  257,
   -1,  125,   91,   -1,   -1,   94,   -1,  257,   -1,   -1,
  268,  269,  270,  271,  272,  273,   -1,   91,  268,  269,
  270,  271,  272,  273,   -1,   91,   -1,  124,   94,   -1,
   -1,   -1,   -1,   -1,    3,    4,  125,    6,    7,    8,
    9,   10,   11,   12,   91,   -1,   -1,   -1,   -1,  123,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   28,
   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
   39,   40,   -1,   42,   43,   44,  123,   46,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   65,   -1,   -1,   -1,
   -1,  257,   71,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  268,  269,  270,  271,
  272,  273,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  268,  269,  270,  271,  272,  273,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  268,
  269,  270,  271,  272,  273,  268,  269,  270,  271,  272,
  273,  268,  269,  270,  271,  272,  273,   -1,   -1,  268,
  269,  270,  271,  272,  273,  259,  260,  261,  262,  263,
   -1,  265,  266,  267,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  268,  269,  270,  271,  272,  273,   -1,   -1,
   -1,   -1,  259,  260,  261,  262,  263,   -1,  265,  266,
  267,
};
#define YYFINAL 13
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 273
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,0,0,0,0,0,0,0,
0,0,0,"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,
"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'","'|'",
"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,"IN","TO","IDENT","REAL","AVG","SUM","LET","NEG",
"LEN","ISNAN","NOT","LT","LE","GT","GE","EQ","NE",
};
char *yyrule[] = {
"$accept : top",
"top : expr",
"expr : '(' expr ')'",
"expr : '{' vector '}'",
"expr : '{' IDENT IN expr '|' expr '}'",
"expr : '(' expr ',' expr ')'",
"expr : '(' expr ',' expr ']'",
"expr : '[' expr ',' expr ')'",
"expr : '[' expr ',' expr ']'",
"expr : expr '+' expr",
"expr : expr '-' expr",
"expr : '-' expr",
"expr : expr '*' expr",
"expr : expr '/' expr",
"expr : expr '^' expr",
"expr : expr LT expr",
"expr : expr LE expr",
"expr : expr GT expr",
"expr : expr GE expr",
"expr : expr EQ expr",
"expr : expr NE expr",
"expr : expr '[' expr ']'",
"expr : LET letexpr",
"expr : NOT expr",
"expr : SUM expr",
"expr : AVG expr",
"expr : LEN expr",
"expr : ISNAN expr",
"expr : IDENT",
"expr : REAL",
"letexpr : IDENT '=' expr ',' letexpr",
"letexpr : IDENT '=' expr IN expr",
"vector : expr",
"vector : vector ',' expr",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 273 "gram.y"

node_t root;

void
yyerror(msg)
	const char *msg;
{
	extern int lexpos;

	show_error(lexpos, msg);
}
#line 339 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 38 "gram.y"
{ root = yyvsp[0].node; }
break;
case 2:
#line 42 "gram.y"
{ yyval.node = yyvsp[-1].node; }
break;
case 3:
#line 45 "gram.y"
{ yyval.node = yyvsp[-1].node; }
break;
case 4:
#line 48 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_GEN;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->ident = yyvsp[-5].ident;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 5:
#line 56 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = 0;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 6:
#line 64 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_UPPER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 7:
#line 72 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_LOWER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 8:
#line 80 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 9:
#line 88 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_ADD;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 10:
#line 96 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_SUB;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 11:
#line 104 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_SUB;
		  yyval.node->pos = yyvsp[-1].pos;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = new_node(0);
		  yyval.node->expr[0]->type = NODETYPE_REAL;
		  yyval.node->expr[0]->real = 0.0;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 12:
#line 114 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_MUL;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 13:
#line 122 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_DIV;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 14:
#line 130 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_EXP;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 15:
#line 138 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_LT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 16:
#line 146 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_LE;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 17:
#line 154 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_GT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 18:
#line 162 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_GE;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 19:
#line 170 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_EQ;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 20:
#line 178 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_NE;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 21:
#line 186 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_INDEX;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 22:
#line 194 "gram.y"
{ yyval.node = yyvsp[0].node; }
break;
case 23:
#line 197 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_NOT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 24:
#line 204 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_SUM;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 25:
#line 210 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_AVG;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 26:
#line 216 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_LEN;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 27:
#line 222 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_ISNAN;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 28:
#line 229 "gram.y"
{ yyval.node = new_node(0);
		  yyval.node->type = NODETYPE_IDENT;
		  yyval.node->pos = -1;
		  yyval.node->ident = yyvsp[0].ident; }
break;
case 29:
#line 235 "gram.y"
{ yyval.node = new_node(0);
		  yyval.node->pos = -1;
		  yyval.node->type = NODETYPE_REAL;
		  yyval.node->real = yyvsp[0].real; }
break;
case 30:
#line 244 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_LET;
		  yyval.node->pos = yyvsp[-3].pos;
		  yyval.node->ident = yyvsp[-4].ident;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 31:
#line 251 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-3].pos;
		  yyval.node->type = NODETYPE_LET;
		  yyval.node->ident = yyvsp[-4].ident;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 32:
#line 260 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[0].node->pos;
		  yyval.node->type = NODETYPE_VEC1;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 33:
#line 265 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_VEC2;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
#line 744 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
