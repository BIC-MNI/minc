
/*  A Bison parser, made from gram.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	IN	257
#define	TO	258
#define	IDENT	259
#define	REAL	260
#define	AVG	261
#define	PROD	262
#define	SUM	263
#define	LET	264
#define	NEG	265
#define	LEN	266
#define	MAX	267
#define	MIN	268
#define	ISNAN	269
#define	SQRT	270
#define	ABS	271
#define	EXP	272
#define	LOG	273
#define	SIN	274
#define	COS	275
#define	CLAMP	276
#define	SEGMENT	277
#define	LT	278
#define	LE	279
#define	GT	280
#define	GE	281
#define	EQ	282
#define	NE	283
#define	NOT	284
#define	AND	285
#define	OR	286
#define	IF	287
#define	ELSE	288
#define	FOR	289

#line 1 "gram.y"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "node.h"

/* Avoid problems with conflicting declarations */
void yyerror(const char *msg);

#line 11 "gram.y"
typedef union{
int      pos;
node_t   node;
float    real;
ident_t  ident;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		143
#define	YYFLAG		-32768
#define	YYNTBASE	54

#define YYTRANSLATE(x) ((unsigned)(x) <= 289 ? yytranslate[x] : 59)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    40,
    41,    38,    36,    49,    37,    44,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    52,    51,     2,
    45,     2,    53,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    42,     2,    43,    46,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    47,    50,    48,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     9,    11,    15,    19,    23,    31,    37,
    43,    49,    55,    59,    63,    66,    70,    74,    78,    82,
    86,    90,    94,    98,   102,   106,   110,   115,   119,   122,
   125,   128,   131,   134,   137,   140,   143,   146,   149,   152,
   155,   158,   161,   164,   173,   182,   188,   200,   208,   216,
   218,   220,   226,   232,   234
};

static const short yyrhs[] = {    55,
     0,    56,    51,    55,     0,    56,    51,     0,    56,     0,
    40,    56,    41,     0,    47,    55,    48,     0,    42,    58,
    43,     0,    47,     5,     3,    56,    50,    56,    48,     0,
    40,    56,    52,    56,    41,     0,    40,    56,    52,    56,
    43,     0,    42,    56,    52,    56,    41,     0,    42,    56,
    52,    56,    43,     0,    56,    36,    56,     0,    56,    37,
    56,     0,    37,    56,     0,    56,    38,    56,     0,    56,
    39,    56,     0,    56,    46,    56,     0,    56,    24,    56,
     0,    56,    25,    56,     0,    56,    26,    56,     0,    56,
    27,    56,     0,    56,    28,    56,     0,    56,    29,    56,
     0,    56,    31,    56,     0,    56,    32,    56,     0,    56,
    42,    56,    43,     0,     5,    45,    56,     0,    10,    57,
     0,    30,    56,     0,     9,    56,     0,     8,    56,     0,
     7,    56,     0,    12,    56,     0,    13,    56,     0,    14,
    56,     0,    15,    56,     0,    16,    56,     0,    17,    56,
     0,    18,    56,     0,    19,    56,     0,    20,    56,     0,
    21,    56,     0,    22,    40,    56,    49,    56,    49,    56,
    41,     0,    23,    40,    56,    49,    56,    49,    56,    41,
     0,    56,    53,    56,    52,    56,     0,    33,    40,    56,
    41,    47,    55,    48,    34,    47,    55,    48,     0,    33,
    40,    56,    41,    47,    55,    48,     0,    35,     5,     3,
    56,    47,    55,    48,     0,     5,     0,     6,     0,     5,
    45,    56,    49,    57,     0,     5,    45,    56,     3,    56,
     0,    56,     0,    58,    49,    56,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    48,    52,    59,    62,    65,    68,    71,    74,    82,    90,
    98,   106,   114,   122,   130,   140,   148,   156,   164,   172,
   180,   188,   196,   204,   212,   220,   228,   235,   242,   245,
   252,   258,   264,   270,   276,   282,   288,   295,   302,   309,
   316,   323,   330,   337,   346,   355,   363,   371,   378,   386,
   392,   401,   408,   417,   422
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","IN","TO",
"IDENT","REAL","AVG","PROD","SUM","LET","NEG","LEN","MAX","MIN","ISNAN","SQRT",
"ABS","EXP","LOG","SIN","COS","CLAMP","SEGMENT","LT","LE","GT","GE","EQ","NE",
"NOT","AND","OR","IF","ELSE","FOR","'+'","'-'","'*'","'/'","'('","')'","'['",
"']'","'.'","'='","'^'","'{'","'}'","','","'|'","';'","':'","'?'","top","exprlist",
"expr","letexpr","vector", NULL
};
#endif

static const short yyr1[] = {     0,
    54,    55,    55,    55,    56,    56,    56,    56,    56,    56,
    56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
    56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
    56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
    56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
    56,    57,    57,    58,    58
};

static const short yyr2[] = {     0,
     1,     3,     2,     1,     3,     3,     3,     7,     5,     5,
     5,     5,     3,     3,     2,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     4,     3,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     8,     8,     5,    11,     7,     7,     1,
     1,     5,     5,     1,     3
};

static const short yydefact[] = {     0,
    50,    51,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     1,     4,     0,    33,    32,
    31,     0,    29,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,     0,     0,    30,     0,     0,    15,     0,
    54,     0,    50,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     3,     0,
    28,     0,     0,     0,     0,     0,     5,     0,     0,     7,
     0,     0,     6,    19,    20,    21,    22,    23,    24,    25,
    26,    13,    14,    16,    17,     0,    18,     2,     0,     0,
     0,     0,     0,     0,     0,     0,    55,     0,    27,     0,
     0,     0,     0,     0,     0,     0,     9,    10,    11,    12,
     0,    46,    53,    52,     0,     0,     0,     0,     0,     0,
     0,    48,    49,     8,    44,    45,     0,     0,     0,    47,
     0,     0,     0
};

static const short yydefgoto[] = {   141,
    26,    27,    33,    52
};

static const short yypact[] = {   149,
   -14,-32768,   149,   149,   149,    28,   149,   149,   149,   149,
   149,   149,   149,   149,   149,   149,    -5,    -2,   149,    -1,
    32,   149,   149,   149,   192,-32768,   292,   149,     1,     1,
     1,     0,-32768,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,   149,   149,   -12,   149,    37,   -12,   216,
   322,   -26,    21,     2,   149,   149,   149,   149,   149,   149,
   149,   149,   149,   149,   149,   149,   149,   149,   149,   149,
   684,   149,   352,   382,   412,   149,-32768,   149,   149,-32768,
   149,   149,-32768,   -10,   -10,   -10,   -10,   753,   753,   730,
   707,   -24,   -24,   -12,   -12,   435,   -12,-32768,   458,   100,
   149,   149,     4,   488,   246,   269,   684,   518,-32768,   149,
   149,    28,   548,   578,   149,   149,-32768,-32768,-32768,-32768,
   149,   684,   684,-32768,   149,   149,    20,    22,   608,   638,
   661,    12,-32768,-32768,-32768,-32768,    24,   149,    26,-32768,
    48,    49,-32768
};

static const short yypgoto[] = {-32768,
   -22,    -3,   -40,-32768
};


#define	YYLAST		799


static const short yytable[] = {    29,
    30,    31,    54,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    65,    66,    46,    80,    67,    49,    50,
    51,    68,    81,    82,    71,    63,    64,    65,    66,    67,
    28,    67,    32,    68,    44,    68,    48,    45,    47,    76,
    73,    74,    67,    75,    72,   137,    98,   142,   143,    83,
   115,    84,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,    95,    96,    97,    28,    99,   132,   100,   133,
   138,   124,   104,   140,   105,   106,     0,   107,   108,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   127,   128,     0,     0,     0,   113,   114,     0,
     0,     0,   111,     0,     0,     0,   122,   123,     0,     0,
     0,     0,     0,     0,     0,   139,     0,   129,     0,     0,
     0,   130,   131,    55,    56,    57,    58,    59,    60,     0,
    61,    62,     0,     0,     0,    63,    64,    65,    66,     0,
     0,    67,     0,     0,     0,    68,     0,     0,   112,     0,
     0,     0,    70,     1,     2,     3,     4,     5,     6,     0,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,     0,     0,     0,     0,     0,     0,    19,     0,
     0,    20,     0,    21,     0,    22,     0,     0,    23,     0,
    24,     0,     0,     0,     0,    25,    53,     2,     3,     4,
     5,     6,     0,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,     0,     0,     0,     0,     0,
     0,    19,     0,     0,    20,     0,    21,     0,    22,     0,
     0,    23,     0,    24,     0,     0,     0,     0,    25,    55,
    56,    57,    58,    59,    60,     0,    61,    62,     0,     0,
     0,    63,    64,    65,    66,     0,    77,    67,     0,     0,
     0,    68,     0,     0,     0,     0,     0,    78,    70,    55,
    56,    57,    58,    59,    60,     0,    61,    62,     0,     0,
     0,    63,    64,    65,    66,     0,   117,    67,   118,     0,
     0,    68,    55,    56,    57,    58,    59,    60,    70,    61,
    62,     0,     0,     0,    63,    64,    65,    66,     0,   119,
    67,   120,     0,     0,    68,    55,    56,    57,    58,    59,
    60,    70,    61,    62,     0,     0,     0,    63,    64,    65,
    66,     0,     0,    67,     0,     0,     0,    68,     0,     0,
     0,     0,    69,     0,    70,    55,    56,    57,    58,    59,
    60,     0,    61,    62,     0,     0,     0,    63,    64,    65,
    66,     0,     0,    67,     0,     0,     0,    68,     0,     0,
     0,     0,     0,    79,    70,    55,    56,    57,    58,    59,
    60,     0,    61,    62,     0,     0,     0,    63,    64,    65,
    66,     0,     0,    67,     0,     0,     0,    68,     0,     0,
   101,     0,     0,     0,    70,    55,    56,    57,    58,    59,
    60,     0,    61,    62,     0,     0,     0,    63,    64,    65,
    66,     0,     0,    67,     0,     0,     0,    68,     0,     0,
   102,     0,     0,     0,    70,    55,    56,    57,    58,    59,
    60,     0,    61,    62,     0,     0,     0,    63,    64,    65,
    66,     0,   103,    67,     0,     0,     0,    68,    55,    56,
    57,    58,    59,    60,    70,    61,    62,     0,     0,     0,
    63,    64,    65,    66,     0,     0,    67,   109,     0,     0,
    68,    55,    56,    57,    58,    59,    60,    70,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,     0,    67,
     0,     0,     0,    68,     0,     0,     0,     0,     0,   110,
    70,    55,    56,    57,    58,    59,    60,     0,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,     0,    67,
     0,     0,     0,    68,   116,     0,     0,     0,     0,     0,
    70,    55,    56,    57,    58,    59,    60,     0,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,     0,    67,
     0,     0,     0,    68,     0,     0,     0,   121,     0,     0,
    70,    55,    56,    57,    58,    59,    60,     0,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,     0,    67,
     0,     0,     0,    68,     0,     0,   125,     0,     0,     0,
    70,    55,    56,    57,    58,    59,    60,     0,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,     0,    67,
     0,     0,     0,    68,     0,     0,   126,     0,     0,     0,
    70,    55,    56,    57,    58,    59,    60,     0,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,     0,    67,
     0,     0,     0,    68,     0,   134,     0,     0,     0,     0,
    70,    55,    56,    57,    58,    59,    60,     0,    61,    62,
     0,     0,     0,    63,    64,    65,    66,     0,   135,    67,
     0,     0,     0,    68,    55,    56,    57,    58,    59,    60,
    70,    61,    62,     0,     0,     0,    63,    64,    65,    66,
     0,   136,    67,     0,     0,     0,    68,    55,    56,    57,
    58,    59,    60,    70,    61,    62,     0,     0,     0,    63,
    64,    65,    66,     0,     0,    67,     0,     0,     0,    68,
    55,    56,    57,    58,    59,    60,    70,    61,     0,     0,
     0,     0,    63,    64,    65,    66,     0,     0,    67,     0,
     0,     0,    68,    55,    56,    57,    58,    59,    60,     0,
     0,     0,     0,     0,     0,    63,    64,    65,    66,     0,
     0,    67,     0,     0,     0,    68,    55,    56,    57,    58,
     0,     0,     0,     0,     0,     0,     0,     0,    63,    64,
    65,    66,     0,     0,    67,     0,     0,     0,    68
};

static const short yycheck[] = {     3,
     4,     5,    25,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    38,    39,    19,    43,    42,    22,    23,
    24,    46,    49,     3,    28,    36,    37,    38,    39,    42,
    45,    42,     5,    46,    40,    46,     5,    40,    40,     3,
    44,    45,    42,    47,    45,    34,    69,     0,     0,    48,
    47,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    45,    70,    48,    72,    48,
    47,   112,    76,    48,    78,    79,    -1,    81,    82,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   115,   116,    -1,    -1,    -1,   101,   102,    -1,
    -1,    -1,     3,    -1,    -1,    -1,   110,   111,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   138,    -1,   121,    -1,    -1,
    -1,   125,   126,    24,    25,    26,    27,    28,    29,    -1,
    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,    49,    -1,
    -1,    -1,    53,     5,     6,     7,     8,     9,    10,    -1,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,    30,    -1,
    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    40,    -1,
    42,    -1,    -1,    -1,    -1,    47,     5,     6,     7,     8,
     9,    10,    -1,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    -1,    -1,    -1,    -1,    -1,
    -1,    30,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,
    -1,    40,    -1,    42,    -1,    -1,    -1,    -1,    47,    24,
    25,    26,    27,    28,    29,    -1,    31,    32,    -1,    -1,
    -1,    36,    37,    38,    39,    -1,    41,    42,    -1,    -1,
    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,    53,    24,
    25,    26,    27,    28,    29,    -1,    31,    32,    -1,    -1,
    -1,    36,    37,    38,    39,    -1,    41,    42,    43,    -1,
    -1,    46,    24,    25,    26,    27,    28,    29,    53,    31,
    32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,
    42,    43,    -1,    -1,    46,    24,    25,    26,    27,    28,
    29,    53,    31,    32,    -1,    -1,    -1,    36,    37,    38,
    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
    -1,    -1,    51,    -1,    53,    24,    25,    26,    27,    28,
    29,    -1,    31,    32,    -1,    -1,    -1,    36,    37,    38,
    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
    -1,    -1,    -1,    52,    53,    24,    25,    26,    27,    28,
    29,    -1,    31,    32,    -1,    -1,    -1,    36,    37,    38,
    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
    49,    -1,    -1,    -1,    53,    24,    25,    26,    27,    28,
    29,    -1,    31,    32,    -1,    -1,    -1,    36,    37,    38,
    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
    49,    -1,    -1,    -1,    53,    24,    25,    26,    27,    28,
    29,    -1,    31,    32,    -1,    -1,    -1,    36,    37,    38,
    39,    -1,    41,    42,    -1,    -1,    -1,    46,    24,    25,
    26,    27,    28,    29,    53,    31,    32,    -1,    -1,    -1,
    36,    37,    38,    39,    -1,    -1,    42,    43,    -1,    -1,
    46,    24,    25,    26,    27,    28,    29,    53,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,
    53,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
    -1,    -1,    -1,    46,    47,    -1,    -1,    -1,    -1,    -1,
    53,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
    -1,    -1,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
    53,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
    -1,    -1,    -1,    46,    -1,    -1,    49,    -1,    -1,    -1,
    53,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
    -1,    -1,    -1,    46,    -1,    -1,    49,    -1,    -1,    -1,
    53,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,
    53,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
    -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,    42,
    -1,    -1,    -1,    46,    24,    25,    26,    27,    28,    29,
    53,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
    -1,    41,    42,    -1,    -1,    -1,    46,    24,    25,    26,
    27,    28,    29,    53,    31,    32,    -1,    -1,    -1,    36,
    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
    24,    25,    26,    27,    28,    29,    53,    31,    -1,    -1,
    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
    -1,    -1,    46,    24,    25,    26,    27,    28,    29,    -1,
    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
    -1,    42,    -1,    -1,    -1,    46,    24,    25,    26,    27,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 49 "gram.y"
{ root = yyvsp[0].node; ;
    break;}
case 2:
#line 53 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_EXPRLIST;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 3:
#line 60 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 4:
#line 63 "gram.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 5:
#line 66 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 6:
#line 69 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 7:
#line 72 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 8:
#line 75 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_GEN;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->ident = yyvsp[-5].ident;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 9:
#line 83 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = 0;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 10:
#line 91 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 11:
#line 99 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 12:
#line 107 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 13:
#line 115 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_ADD;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 14:
#line 123 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 15:
#line 131 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = new_scalar_node(0);
        yyval.node->expr[0]->type = NODETYPE_REAL;
        yyval.node->expr[0]->real = 0.0;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 16:
#line 141 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MUL;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 17:
#line 149 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_DIV;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 18:
#line 157 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_POW;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 19:
#line 165 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 20:
#line 173 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 21:
#line 181 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 22:
#line 189 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 23:
#line 197 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_EQ;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 24:
#line 205 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_NE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 25:
#line 213 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_AND;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 26:
#line 221 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_OR;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 27:
#line 229 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_INDEX;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 28:
#line 236 "gram.y"
{ yyval.node = new_node(1, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_ASSIGN;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->ident = yyvsp[-2].ident;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 29:
#line 243 "gram.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 30:
#line 246 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_NOT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 31:
#line 253 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SUM;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 32:
#line 259 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_PROD;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 33:
#line 265 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_AVG;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 34:
#line 271 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LEN;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 35:
#line 277 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MAX;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 36:
#line 283 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MIN;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 37:
#line 289 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ISNAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 38:
#line 296 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SQRT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 39:
#line 303 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ABS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 40:
#line 310 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_EXP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 41:
#line 317 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LOG;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 42:
#line 324 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 43:
#line 331 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_COS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 44:
#line 338 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_CLAMP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 45:
#line 347 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_SEGMENT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 46:
#line 356 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; ;
    break;}
case 47:
#line 364 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-8].node));
        yyval.node->pos = yyvsp[-10].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-8].node;
        yyval.node->expr[1] = yyvsp[-5].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 48:
#line 372 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[-4].node));
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 49:
#line 379 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_FOR;
        yyval.node->ident = yyvsp[-5].ident;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 50:
#line 387 "gram.y"
{ yyval.node = new_node(0, ident_is_scalar(yyvsp[0].ident));
        yyval.node->type = NODETYPE_IDENT;
        yyval.node->pos = -1;
        yyval.node->ident = yyvsp[0].ident; ;
    break;}
case 51:
#line 393 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = yyvsp[0].real; ;
    break;}
case 52:
#line 402 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LET;
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 53:
#line 409 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_LET;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 54:
#line 418 "gram.y"
{ yyval.node = new_vector_node(1);
        yyval.node->pos = yyvsp[0].node->pos;
        yyval.node->type = NODETYPE_VEC1;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 55:
#line 423 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_VEC2;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 430 "gram.y"


node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}
