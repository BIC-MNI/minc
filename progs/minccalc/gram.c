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
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    2,    2,    3,    3,
};
short yylen[] = {                                         2,
    1,    3,    3,    7,    5,    5,    5,    5,    3,    3,
    2,    3,    3,    3,    4,    2,    2,    2,    2,    1,
    1,    5,    5,    1,    3,
};
short yydefred[] = {                                      0,
   20,   21,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   16,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    2,    0,    0,    0,    3,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   15,    0,    0,
    5,    6,    7,    8,    0,    0,   22,    0,    4,
};
short yydgoto[] = {                                      11,
   12,   16,   23,
};
short yysindex[] = {                                     -6,
    0,    0,   -6,   -6, -257,   -6,   -6,   -6,   -6,   44,
    0,  309,  -87,  -87,  -56,    0,  -87,  -91,  240,  282,
 -247,  309,  -38,   -6,   -6,   -6,   -6,   -6,   -6,   -6,
    0,   -6,   -6,   -6,    0,   -6,   57,   57,  -91,  -91,
  290,  -91,  128,  219,  226,  153,  309,    0,   -6, -257,
    0,    0,    0,    0,   -6,  309,    0,  145,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   11,    1,    8,    0,    0,   15,   22,    0,    0,
  138,  -37,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   93,  100,   29,   36,
    0,  121,    0,    0,    0,    0,  -35,    0,    0,    0,
    0,    0,    0,    0,    0,   86,    0,    0,    0,
};
short yygindex[] = {                                      0,
  338,  -36,    0,
};
#define YYTABLESIZE 403
short yytable[] = {                                      28,
   18,   15,   29,   28,   30,   36,   24,   17,   25,   34,
    1,    0,    0,   57,   19,    0,    0,    0,    0,    0,
    0,   11,    0,    0,    0,    0,    0,    0,   12,    0,
    0,    0,    0,    8,    0,   13,    0,    0,    7,    0,
    0,   18,   18,   18,   18,   18,    0,   18,   17,   17,
   17,   17,   17,    0,   17,   19,   19,   19,   19,   19,
    0,   19,   11,   11,   11,   11,   11,    0,   11,   12,
   12,   12,   12,   12,    0,   12,   13,   13,   13,   13,
   13,    0,   13,    8,    9,   23,   35,   24,    7,   25,
    0,    0,    9,   18,   18,    0,    0,    0,   26,   10,
   17,   17,    0,   27,    0,    0,    0,   19,   19,    0,
    0,    0,    0,    0,   11,    0,   10,    0,    0,    0,
   14,   12,    0,    0,   18,   18,   23,    0,   13,   23,
    0,   17,   17,    9,    9,    9,    9,    9,   19,   19,
   10,    0,   10,   10,   10,   11,   11,   28,    0,    0,
   29,    0,   12,   12,    0,    0,    0,    0,    0,   13,
   13,   14,   14,   14,   14,   14,   10,   14,    0,   26,
   24,   50,   25,    0,   27,    0,    0,    0,   23,   20,
   20,   20,   20,    0,   20,    9,   26,   24,    0,   25,
    0,   27,   10,    0,   26,   24,    0,   25,    0,   27,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   23,
   23,    0,    0,   14,    0,    0,    9,    9,   28,    0,
    0,   29,    0,   10,   10,    0,    0,    0,   20,    0,
    0,   20,    0,    0,    0,   28,    0,    0,   29,    0,
    0,    0,    0,   28,   14,   14,   29,    0,    0,    0,
    0,    0,    1,    2,    3,    4,    5,   18,    6,   51,
   26,   24,   20,   25,   17,   27,   53,   26,   24,   59,
   25,   19,   27,    0,    0,    0,   55,    0,   11,    0,
   31,   26,   24,   32,   25,   12,   27,    0,    0,    0,
    0,    0,   13,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   21,    2,    3,    4,    5,    0,    6,   28,
    0,   52,   29,    0,    0,    0,   28,    0,   54,   29,
    0,    0,    0,   26,   24,   33,   25,    0,   27,    0,
   28,   26,   24,   29,   25,    0,   27,    0,    0,    0,
   13,   14,   23,   17,   18,   19,   20,   22,    0,    9,
   26,   24,    0,   25,    0,   27,   10,    0,    0,    0,
    0,   37,   38,   39,   40,   41,   42,   43,    0,   44,
   45,   46,   28,   47,    0,   29,    0,   14,    0,    0,
   28,    0,   48,   29,   49,    0,   56,    0,    0,    0,
    0,    0,   58,    0,    0,    0,    0,    0,    0,   28,
    0,    0,   29,
};
short yycheck[] = {                                      91,
    0,  259,   94,   91,   61,   44,   44,    0,   44,  257,
    0,   -1,   -1,   50,    0,   -1,   -1,   -1,   -1,   -1,
   -1,    0,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,
   -1,   -1,   -1,   40,   -1,    0,   -1,   -1,   45,   -1,
   -1,   41,   42,   43,   44,   45,   -1,   47,   41,   42,
   43,   44,   45,   -1,   47,   41,   42,   43,   44,   45,
   -1,   47,   41,   42,   43,   44,   45,   -1,   47,   41,
   42,   43,   44,   45,   -1,   47,   41,   42,   43,   44,
   45,   -1,   47,   40,   91,    0,  125,  125,   45,  125,
   -1,   -1,    0,   93,   94,   -1,   -1,   -1,   42,    0,
   93,   94,   -1,   47,   -1,   -1,   -1,   93,   94,   -1,
   -1,   -1,   -1,   -1,   93,   -1,  123,   -1,   -1,   -1,
    0,   93,   -1,   -1,  124,  125,   41,   -1,   93,   44,
   -1,  124,  125,   41,   91,   43,   44,   45,  124,  125,
   41,   -1,   43,   44,   45,  124,  125,   91,   -1,   -1,
   94,   -1,  124,  125,   -1,   -1,   -1,   -1,   -1,  124,
  125,   41,   42,   43,   44,   45,  123,   47,   -1,   42,
   43,   44,   45,   -1,   47,   -1,   -1,   -1,   93,   42,
   43,   44,   45,   -1,   47,   93,   42,   43,   -1,   45,
   -1,   47,   93,   -1,   42,   43,   -1,   45,   -1,   47,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  124,
  125,   -1,   -1,   93,   -1,   -1,  124,  125,   91,   -1,
   -1,   94,   -1,  124,  125,   -1,   -1,   -1,   91,   -1,
   -1,   94,   -1,   -1,   -1,   91,   -1,   -1,   94,   -1,
   -1,   -1,   -1,   91,  124,  125,   94,   -1,   -1,   -1,
   -1,   -1,  259,  260,  261,  262,  263,  257,  265,   41,
   42,   43,  125,   45,  257,   47,   41,   42,   43,  125,
   45,  257,   47,   -1,   -1,   -1,  124,   -1,  257,   -1,
   41,   42,   43,   44,   45,  257,   47,   -1,   -1,   -1,
   -1,   -1,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  259,  260,  261,  262,  263,   -1,  265,   91,
   -1,   93,   94,   -1,   -1,   -1,   91,   -1,   93,   94,
   -1,   -1,   -1,   42,   43,   44,   45,   -1,   47,   -1,
   91,   42,   43,   94,   45,   -1,   47,   -1,   -1,   -1,
    3,    4,  257,    6,    7,    8,    9,   10,   -1,  257,
   42,   43,   -1,   45,   -1,   47,  257,   -1,   -1,   -1,
   -1,   24,   25,   26,   27,   28,   29,   30,   -1,   32,
   33,   34,   91,   36,   -1,   94,   -1,  257,   -1,   -1,
   91,   -1,   93,   94,  257,   -1,   49,   -1,   -1,   -1,
   -1,   -1,   55,   -1,   -1,   -1,   -1,   -1,   -1,   91,
   -1,   -1,   94,
};
#define YYFINAL 11
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 265
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
"LEN",
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
"expr : expr '[' expr ']'",
"expr : LET letexpr",
"expr : SUM expr",
"expr : AVG expr",
"expr : LEN expr",
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
#line 200 "gram.y"

node_t root;

void
yyerror(msg)
	const char *msg;
{
	extern int lexpos;

	show_error(lexpos, msg);
}
#line 241 "y.tab.c"
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
#line 34 "gram.y"
{ root = yyvsp[0].node; }
break;
case 2:
#line 38 "gram.y"
{ yyval.node = yyvsp[-1].node; }
break;
case 3:
#line 41 "gram.y"
{ yyval.node = yyvsp[-1].node; }
break;
case 4:
#line 44 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_GEN;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->ident = yyvsp[-5].ident;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 5:
#line 52 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = 0;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 6:
#line 60 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_UPPER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 7:
#line 68 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_LOWER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 8:
#line 76 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 9:
#line 84 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_ADD;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 10:
#line 91 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_SUB;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 11:
#line 98 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_SUB;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = new_node();
		  yyval.node->expr[0]->type = NODETYPE_REAL;
		  yyval.node->expr[0]->real = 0.0;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 12:
#line 107 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_MUL;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 13:
#line 114 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_DIV;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 14:
#line 121 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_EXP;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 15:
#line 128 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_INDEX;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; }
break;
case 16:
#line 135 "gram.y"
{ yyval.node = yyvsp[0].node; }
break;
case 17:
#line 138 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_SUM;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 18:
#line 144 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_AVG;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 19:
#line 150 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_LEN;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 20:
#line 156 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_IDENT;
		  yyval.node->pos = -1;
		  yyval.node->ident = yyvsp[0].ident; }
break;
case 21:
#line 162 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = -1;
		  yyval.node->type = NODETYPE_REAL;
		  yyval.node->real = yyvsp[0].real; }
break;
case 22:
#line 171 "gram.y"
{ yyval.node = new_node();
		  yyval.node->type = NODETYPE_LET;
		  yyval.node->pos = yyvsp[-3].pos;
		  yyval.node->ident = yyvsp[-4].ident;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 23:
#line 178 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-3].pos;
		  yyval.node->type = NODETYPE_LET;
		  yyval.node->ident = yyvsp[-4].ident;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
case 24:
#line 187 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[0].node->pos;
		  yyval.node->type = NODETYPE_VEC1;
		  yyval.node->expr[0] = yyvsp[0].node; }
break;
case 25:
#line 192 "gram.y"
{ yyval.node = new_node();
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_VEC2;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; }
break;
#line 569 "y.tab.c"
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
