
/*  A Bison parser, made from gram.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NAN	258
#define	IN	259
#define	TO	260
#define	IDENT	261
#define	REAL	262
#define	AVG	263
#define	PROD	264
#define	SUM	265
#define	LET	266
#define	NEG	267
#define	LEN	268
#define	MAX	269
#define	MIN	270
#define	ISNAN	271
#define	SQRT	272
#define	ABS	273
#define	EXP	274
#define	LOG	275
#define	SIN	276
#define	COS	277
#define	TAN	278
#define	ASIN	279
#define	ACOS	280
#define	ATAN	281
#define	CLAMP	282
#define	SEGMENT	283
#define	LT	284
#define	LE	285
#define	GT	286
#define	GE	287
#define	EQ	288
#define	NE	289
#define	NOT	290
#define	AND	291
#define	OR	292
#define	IF	293
#define	ELSE	294
#define	FOR	295

#line 1 "gram.y"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include "node.h"

#define INVALID_VALUE (-DBL_MAX)

/* Avoid problems with conflicting declarations */
void yyerror(const char *msg);

#line 15 "gram.y"
typedef union{
int      pos;
node_t   node;
double   real;
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



#define	YYFINAL		148
#define	YYFLAG		-32768
#define	YYNTBASE	59

#define YYTRANSLATE(x) ((unsigned)(x) <= 295 ? yytranslate[x] : 64)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    45,
    46,    43,    41,    54,    42,    49,    44,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    57,    56,     2,
    50,     2,    58,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    47,     2,    48,    51,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    52,    55,    53,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     9,    11,    15,    19,    23,    31,    37,
    43,    49,    55,    59,    63,    66,    70,    74,    78,    82,
    86,    90,    94,    98,   102,   106,   110,   115,   119,   122,
   125,   128,   131,   134,   137,   140,   143,   146,   149,   152,
   155,   158,   161,   164,   167,   170,   173,   176,   185,   194,
   200,   208,   214,   222,   224,   226,   228,   234,   240,   242
};

static const short yyrhs[] = {    60,
     0,    61,    56,    60,     0,    61,    56,     0,    61,     0,
    45,    61,    46,     0,    52,    60,    53,     0,    47,    63,
    48,     0,    52,     6,     4,    61,    55,    61,    53,     0,
    45,    61,    57,    61,    46,     0,    45,    61,    57,    61,
    48,     0,    47,    61,    57,    61,    46,     0,    47,    61,
    57,    61,    48,     0,    61,    41,    61,     0,    61,    42,
    61,     0,    42,    61,     0,    61,    43,    61,     0,    61,
    44,    61,     0,    61,    51,    61,     0,    61,    29,    61,
     0,    61,    30,    61,     0,    61,    31,    61,     0,    61,
    32,    61,     0,    61,    33,    61,     0,    61,    34,    61,
     0,    61,    36,    61,     0,    61,    37,    61,     0,    61,
    47,    61,    48,     0,     6,    50,    61,     0,    11,    62,
     0,    35,    61,     0,    10,    61,     0,     9,    61,     0,
     8,    61,     0,    13,    61,     0,    14,    61,     0,    15,
    61,     0,    16,    61,     0,    17,    61,     0,    18,    61,
     0,    19,    61,     0,    20,    61,     0,    21,    61,     0,
    22,    61,     0,    23,    61,     0,    24,    61,     0,    25,
    61,     0,    26,    61,     0,    27,    45,    61,    54,    61,
    54,    61,    46,     0,    28,    45,    61,    54,    61,    54,
    61,    46,     0,    61,    58,    61,    57,    61,     0,    38,
    45,    61,    46,    61,    39,    61,     0,    38,    45,    61,
    46,    61,     0,    40,    52,     6,     4,    61,    53,    61,
     0,     6,     0,     7,     0,     3,     0,     6,    50,    61,
    54,    62,     0,     6,    50,    61,     4,    61,     0,    61,
     0,    63,    54,    61,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    53,    57,    64,    67,    70,    73,    76,    79,    87,    95,
   103,   111,   119,   127,   135,   145,   153,   161,   169,   177,
   185,   193,   201,   209,   217,   225,   233,   240,   247,   250,
   257,   263,   269,   275,   281,   287,   293,   300,   307,   314,
   321,   328,   335,   342,   349,   356,   363,   370,   379,   388,
   396,   404,   411,   419,   425,   431,   440,   447,   456,   461
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAN","IN",
"TO","IDENT","REAL","AVG","PROD","SUM","LET","NEG","LEN","MAX","MIN","ISNAN",
"SQRT","ABS","EXP","LOG","SIN","COS","TAN","ASIN","ACOS","ATAN","CLAMP","SEGMENT",
"LT","LE","GT","GE","EQ","NE","NOT","AND","OR","IF","ELSE","FOR","'+'","'-'",
"'*'","'/'","'('","')'","'['","']'","'.'","'='","'^'","'{'","'}'","','","'|'",
"';'","':'","'?'","top","exprlist","expr","letexpr","vector", NULL
};
#endif

static const short yyr1[] = {     0,
    59,    60,    60,    60,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    62,    62,    63,    63
};

static const short yyr2[] = {     0,
     1,     3,     2,     1,     3,     3,     3,     7,     5,     5,
     5,     5,     3,     3,     2,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     4,     3,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     8,     8,     5,
     7,     5,     7,     1,     1,     1,     5,     5,     1,     3
};

static const short yydefact[] = {     0,
    56,    54,    55,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     1,     4,     0,    33,    32,    31,     0,    29,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,     0,     0,    30,     0,     0,    15,     0,    59,
     0,    54,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     3,     0,    28,
     0,     0,     0,     0,     0,     5,     0,     0,     7,     0,
     0,     6,    19,    20,    21,    22,    23,    24,    25,    26,
    13,    14,    16,    17,     0,    18,     2,     0,     0,     0,
     0,     0,     0,     0,     0,    60,     0,    27,     0,     0,
     0,     0,     0,    52,     0,     9,    10,    11,    12,     0,
    50,    58,    57,     0,     0,     0,     0,     0,     0,     0,
    51,    53,     8,    48,    49,     0,     0,     0
};

static const short yydefgoto[] = {   146,
    31,    32,    38,    61
};

static const short yypact[] = {   159,
-32768,   -29,-32768,   159,   159,   159,    21,   159,   159,   159,
   159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
   159,    -8,    -6,   159,    -2,   -18,   159,   159,   159,   206,
-32768,   306,   159,    -3,    -3,    -3,    -5,-32768,    -3,    -3,
    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,
    -3,    -3,   159,   159,    -9,   159,    40,    -9,   230,   336,
   -13,    24,     0,   159,   159,   159,   159,   159,   159,   159,
   159,   159,   159,   159,   159,   159,   159,   159,   159,   721,
   159,   366,   396,   426,    43,-32768,   159,   159,-32768,   159,
   159,-32768,   -11,   -11,   -11,   -11,   767,   767,    70,   744,
   -25,   -25,    -9,    -9,   449,    -9,-32768,   472,   106,   159,
   159,   159,   159,   260,   283,   721,   502,-32768,   159,   159,
    21,   532,   562,   592,   615,-32768,-32768,-32768,-32768,   159,
   721,   721,-32768,   159,   159,   159,   159,   645,   675,   698,
   721,   721,-32768,-32768,-32768,    48,    54,-32768
};

static const short yypgoto[] = {-32768,
   -27,    -4,   -66,-32768
};


#define	YYLAST		818


static const short yytable[] = {    34,
    35,    36,    63,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    74,    75,    55,
    33,    76,    58,    59,    60,    77,    37,    91,    80,    72,
    73,    74,    75,    57,    89,    76,    53,    76,    54,    77,
    90,    77,    56,    76,    81,    85,   113,   147,    82,    83,
   107,    84,    92,   148,   133,     0,     0,     0,     0,    93,
    94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
   104,   105,   106,    33,   108,     0,   109,     0,     0,     0,
     0,     0,   114,   115,     0,   116,   117,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    64,    65,
    66,    67,    68,    69,     0,   122,   123,   124,   125,   120,
    72,    73,    74,    75,   131,   132,    76,     0,     0,     0,
    77,     0,     0,     0,     0,   138,     0,     0,     0,   139,
   140,   141,   142,     0,    64,    65,    66,    67,    68,    69,
     0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
     0,     0,    76,     0,     0,     0,    77,     0,     0,   121,
     0,     1,     0,    79,     2,     3,     4,     5,     6,     7,
     0,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,     0,     0,     0,
     0,     0,     0,    24,     0,     0,    25,     0,    26,     0,
    27,     0,     0,    28,     0,    29,     0,     0,     1,     0,
    30,    62,     3,     4,     5,     6,     7,     0,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,     0,     0,     0,     0,     0,     0,
    24,     0,     0,    25,     0,    26,     0,    27,     0,     0,
    28,     0,    29,     0,     0,     0,     0,    30,    64,    65,
    66,    67,    68,    69,     0,    70,    71,     0,     0,     0,
    72,    73,    74,    75,     0,    86,    76,     0,     0,     0,
    77,     0,     0,     0,     0,     0,    87,    79,    64,    65,
    66,    67,    68,    69,     0,    70,    71,     0,     0,     0,
    72,    73,    74,    75,     0,   126,    76,   127,     0,     0,
    77,    64,    65,    66,    67,    68,    69,    79,    70,    71,
     0,     0,     0,    72,    73,    74,    75,     0,   128,    76,
   129,     0,     0,    77,    64,    65,    66,    67,    68,    69,
    79,    70,    71,     0,     0,     0,    72,    73,    74,    75,
     0,     0,    76,     0,     0,     0,    77,     0,     0,     0,
     0,    78,     0,    79,    64,    65,    66,    67,    68,    69,
     0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
     0,     0,    76,     0,     0,     0,    77,     0,     0,     0,
     0,     0,    88,    79,    64,    65,    66,    67,    68,    69,
     0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
     0,     0,    76,     0,     0,     0,    77,     0,     0,   110,
     0,     0,     0,    79,    64,    65,    66,    67,    68,    69,
     0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
     0,     0,    76,     0,     0,     0,    77,     0,     0,   111,
     0,     0,     0,    79,    64,    65,    66,    67,    68,    69,
     0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
     0,   112,    76,     0,     0,     0,    77,    64,    65,    66,
    67,    68,    69,    79,    70,    71,     0,     0,     0,    72,
    73,    74,    75,     0,     0,    76,   118,     0,     0,    77,
    64,    65,    66,    67,    68,    69,    79,    70,    71,     0,
     0,     0,    72,    73,    74,    75,     0,     0,    76,     0,
     0,     0,    77,     0,     0,     0,     0,     0,   119,    79,
    64,    65,    66,    67,    68,    69,     0,    70,    71,     0,
     0,     0,    72,    73,    74,    75,     0,     0,    76,     0,
     0,     0,    77,     0,     0,     0,   130,     0,     0,    79,
    64,    65,    66,    67,    68,    69,     0,    70,    71,     0,
     0,     0,    72,    73,    74,    75,     0,     0,    76,     0,
     0,     0,    77,     0,     0,   134,     0,     0,     0,    79,
    64,    65,    66,    67,    68,    69,     0,    70,    71,     0,
     0,     0,    72,    73,    74,    75,     0,     0,    76,     0,
     0,     0,    77,     0,     0,   135,     0,     0,     0,    79,
    64,    65,    66,    67,    68,    69,     0,    70,    71,     0,
   136,     0,    72,    73,    74,    75,     0,     0,    76,     0,
     0,     0,    77,    64,    65,    66,    67,    68,    69,    79,
    70,    71,     0,     0,     0,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,    77,     0,   137,     0,     0,
     0,     0,    79,    64,    65,    66,    67,    68,    69,     0,
    70,    71,     0,     0,     0,    72,    73,    74,    75,     0,
     0,    76,     0,     0,     0,    77,     0,   143,     0,     0,
     0,     0,    79,    64,    65,    66,    67,    68,    69,     0,
    70,    71,     0,     0,     0,    72,    73,    74,    75,     0,
   144,    76,     0,     0,     0,    77,    64,    65,    66,    67,
    68,    69,    79,    70,    71,     0,     0,     0,    72,    73,
    74,    75,     0,   145,    76,     0,     0,     0,    77,    64,
    65,    66,    67,    68,    69,    79,    70,    71,     0,     0,
     0,    72,    73,    74,    75,     0,     0,    76,     0,     0,
     0,    77,    64,    65,    66,    67,    68,    69,    79,    70,
     0,     0,     0,     0,    72,    73,    74,    75,     0,     0,
    76,     0,     0,     0,    77,    64,    65,    66,    67,     0,
     0,     0,     0,     0,     0,     0,     0,    72,    73,    74,
    75,     0,     0,    76,     0,     0,     0,    77
};

static const short yycheck[] = {     4,
     5,     6,    30,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    43,    44,    24,
    50,    47,    27,    28,    29,    51,     6,     4,    33,    41,
    42,    43,    44,    52,    48,    47,    45,    47,    45,    51,
    54,    51,    45,    47,    50,     6,     4,     0,    53,    54,
    78,    56,    53,     0,   121,    -1,    -1,    -1,    -1,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    50,    79,    -1,    81,    -1,    -1,    -1,
    -1,    -1,    87,    88,    -1,    90,    91,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
    31,    32,    33,    34,    -1,   110,   111,   112,   113,     4,
    41,    42,    43,    44,   119,   120,    47,    -1,    -1,    -1,
    51,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,   134,
   135,   136,   137,    -1,    29,    30,    31,    32,    33,    34,
    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,
    -1,     3,    -1,    58,     6,     7,     8,     9,    10,    11,
    -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
    -1,    -1,    -1,    35,    -1,    -1,    38,    -1,    40,    -1,
    42,    -1,    -1,    45,    -1,    47,    -1,    -1,     3,    -1,
    52,     6,     7,     8,     9,    10,    11,    -1,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,
    35,    -1,    -1,    38,    -1,    40,    -1,    42,    -1,    -1,
    45,    -1,    47,    -1,    -1,    -1,    -1,    52,    29,    30,
    31,    32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,
    41,    42,    43,    44,    -1,    46,    47,    -1,    -1,    -1,
    51,    -1,    -1,    -1,    -1,    -1,    57,    58,    29,    30,
    31,    32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,
    41,    42,    43,    44,    -1,    46,    47,    48,    -1,    -1,
    51,    29,    30,    31,    32,    33,    34,    58,    36,    37,
    -1,    -1,    -1,    41,    42,    43,    44,    -1,    46,    47,
    48,    -1,    -1,    51,    29,    30,    31,    32,    33,    34,
    58,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,
    -1,    56,    -1,    58,    29,    30,    31,    32,    33,    34,
    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,
    -1,    -1,    57,    58,    29,    30,    31,    32,    33,    34,
    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,
    -1,    -1,    -1,    58,    29,    30,    31,    32,    33,    34,
    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,
    -1,    -1,    -1,    58,    29,    30,    31,    32,    33,    34,
    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
    -1,    46,    47,    -1,    -1,    -1,    51,    29,    30,    31,
    32,    33,    34,    58,    36,    37,    -1,    -1,    -1,    41,
    42,    43,    44,    -1,    -1,    47,    48,    -1,    -1,    51,
    29,    30,    31,    32,    33,    34,    58,    36,    37,    -1,
    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,
    -1,    -1,    51,    -1,    -1,    -1,    -1,    -1,    57,    58,
    29,    30,    31,    32,    33,    34,    -1,    36,    37,    -1,
    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,
    -1,    -1,    51,    -1,    -1,    -1,    55,    -1,    -1,    58,
    29,    30,    31,    32,    33,    34,    -1,    36,    37,    -1,
    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,
    -1,    -1,    51,    -1,    -1,    54,    -1,    -1,    -1,    58,
    29,    30,    31,    32,    33,    34,    -1,    36,    37,    -1,
    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,
    -1,    -1,    51,    -1,    -1,    54,    -1,    -1,    -1,    58,
    29,    30,    31,    32,    33,    34,    -1,    36,    37,    -1,
    39,    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,
    -1,    -1,    51,    29,    30,    31,    32,    33,    34,    58,
    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
    -1,    47,    -1,    -1,    -1,    51,    -1,    53,    -1,    -1,
    -1,    -1,    58,    29,    30,    31,    32,    33,    34,    -1,
    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
    -1,    47,    -1,    -1,    -1,    51,    -1,    53,    -1,    -1,
    -1,    -1,    58,    29,    30,    31,    32,    33,    34,    -1,
    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
    46,    47,    -1,    -1,    -1,    51,    29,    30,    31,    32,
    33,    34,    58,    36,    37,    -1,    -1,    -1,    41,    42,
    43,    44,    -1,    46,    47,    -1,    -1,    -1,    51,    29,
    30,    31,    32,    33,    34,    58,    36,    37,    -1,    -1,
    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,    -1,
    -1,    51,    29,    30,    31,    32,    33,    34,    58,    36,
    -1,    -1,    -1,    -1,    41,    42,    43,    44,    -1,    -1,
    47,    -1,    -1,    -1,    51,    29,    30,    31,    32,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    -1,    -1,    47,    -1,    -1,    -1,    51
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
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

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

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
     int count;
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
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "bison.simple"

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
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
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
#line 54 "gram.y"
{ root = yyvsp[0].node; ;
    break;}
case 2:
#line 58 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_EXPRLIST;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 3:
#line 65 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 4:
#line 68 "gram.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 5:
#line 71 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 6:
#line 74 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 7:
#line 77 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 8:
#line 80 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_GEN;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->ident = yyvsp[-5].ident;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 9:
#line 88 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = 0;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 10:
#line 96 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 11:
#line 104 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 12:
#line 112 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 13:
#line 120 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_ADD;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 14:
#line 128 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 15:
#line 136 "gram.y"
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
#line 146 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MUL;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 17:
#line 154 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_DIV;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 18:
#line 162 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_POW;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 19:
#line 170 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 20:
#line 178 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 21:
#line 186 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 22:
#line 194 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 23:
#line 202 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_EQ;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 24:
#line 210 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_NE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 25:
#line 218 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_AND;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 26:
#line 226 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_OR;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 27:
#line 234 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_INDEX;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 28:
#line 241 "gram.y"
{ yyval.node = new_node(1, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_ASSIGN;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->ident = yyvsp[-2].ident;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 29:
#line 248 "gram.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 30:
#line 251 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_NOT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 31:
#line 258 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SUM;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 32:
#line 264 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_PROD;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 33:
#line 270 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_AVG;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 34:
#line 276 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LEN;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 35:
#line 282 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MAX;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 36:
#line 288 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MIN;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 37:
#line 294 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ISNAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 38:
#line 301 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SQRT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 39:
#line 308 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ABS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 40:
#line 315 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_EXP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 41:
#line 322 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LOG;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 42:
#line 329 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 43:
#line 336 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_COS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 44:
#line 343 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_TAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 45:
#line 350 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ASIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 46:
#line 357 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ACOS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 47:
#line 364 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ATAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 48:
#line 371 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_CLAMP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 49:
#line 380 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_SEGMENT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 50:
#line 389 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; ;
    break;}
case 51:
#line 397 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; ;
    break;}
case 52:
#line 405 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->pos = yyvsp[-4].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 53:
#line 412 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_FOR;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 54:
#line 420 "gram.y"
{ yyval.node = new_node(0, ident_is_scalar(yyvsp[0].ident));
        yyval.node->type = NODETYPE_IDENT;
        yyval.node->pos = -1;
        yyval.node->ident = yyvsp[0].ident; ;
    break;}
case 55:
#line 426 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = yyvsp[0].real; ;
    break;}
case 56:
#line 432 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = INVALID_VALUE; ;
    break;}
case 57:
#line 441 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LET;
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 58:
#line 448 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_LET;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 59:
#line 457 "gram.y"
{ yyval.node = new_vector_node(1);
        yyval.node->pos = yyvsp[0].node->pos;
        yyval.node->type = NODETYPE_VEC1;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 60:
#line 462 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_VEC2;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "bison.simple"

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
}
#line 469 "gram.y"


node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}
