
/*  A Bison parser, made from gram.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NAN	257
#define	IN	258
#define	TO	259
#define	IDENT	260
#define	REAL	261
#define	AVG	262
#define	PROD	263
#define	SUM	264
#define	LET	265
#define	NEG	266
#define	LEN	267
#define	MAX	268
#define	MIN	269
#define	ISNAN	270
#define	SQRT	271
#define	ABS	272
#define	EXP	273
#define	LOG	274
#define	SIN	275
#define	COS	276
#define	CLAMP	277
#define	SEGMENT	278
#define	LT	279
#define	LE	280
#define	GT	281
#define	GE	282
#define	EQ	283
#define	NE	284
#define	NOT	285
#define	AND	286
#define	OR	287
#define	IF	288
#define	ELSE	289
#define	FOR	290

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



#define	YYFINAL		140
#define	YYFLAG		-32768
#define	YYNTBASE	55

#define YYTRANSLATE(x) ((unsigned)(x) <= 290 ? yytranslate[x] : 60)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    41,
    42,    39,    37,    50,    38,    45,    40,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    53,    52,     2,
    46,     2,    54,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    43,     2,    44,    47,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    48,    51,    49,     2,     2,     2,     2,     2,
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
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     9,    11,    15,    19,    23,    31,    37,
    43,    49,    55,    59,    63,    66,    70,    74,    78,    82,
    86,    90,    94,    98,   102,   106,   110,   115,   119,   122,
   125,   128,   131,   134,   137,   140,   143,   146,   149,   152,
   155,   158,   161,   164,   173,   182,   188,   196,   202,   210,
   212,   214,   216,   222,   228,   230
};

static const short yyrhs[] = {    56,
     0,    57,    52,    56,     0,    57,    52,     0,    57,     0,
    41,    57,    42,     0,    48,    56,    49,     0,    43,    59,
    44,     0,    48,     6,     4,    57,    51,    57,    49,     0,
    41,    57,    53,    57,    42,     0,    41,    57,    53,    57,
    44,     0,    43,    57,    53,    57,    42,     0,    43,    57,
    53,    57,    44,     0,    57,    37,    57,     0,    57,    38,
    57,     0,    38,    57,     0,    57,    39,    57,     0,    57,
    40,    57,     0,    57,    47,    57,     0,    57,    25,    57,
     0,    57,    26,    57,     0,    57,    27,    57,     0,    57,
    28,    57,     0,    57,    29,    57,     0,    57,    30,    57,
     0,    57,    32,    57,     0,    57,    33,    57,     0,    57,
    43,    57,    44,     0,     6,    46,    57,     0,    11,    58,
     0,    31,    57,     0,    10,    57,     0,     9,    57,     0,
     8,    57,     0,    13,    57,     0,    14,    57,     0,    15,
    57,     0,    16,    57,     0,    17,    57,     0,    18,    57,
     0,    19,    57,     0,    20,    57,     0,    21,    57,     0,
    22,    57,     0,    23,    41,    57,    50,    57,    50,    57,
    42,     0,    24,    41,    57,    50,    57,    50,    57,    42,
     0,    57,    54,    57,    53,    57,     0,    34,    41,    57,
    42,    57,    35,    57,     0,    34,    41,    57,    42,    57,
     0,    36,    48,     6,     4,    57,    49,    57,     0,     6,
     0,     7,     0,     3,     0,     6,    46,    57,    50,    58,
     0,     6,    46,    57,     4,    57,     0,    57,     0,    59,
    50,    57,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    53,    57,    64,    67,    70,    73,    76,    79,    87,    95,
   103,   111,   119,   127,   135,   145,   153,   161,   169,   177,
   185,   193,   201,   209,   217,   225,   233,   240,   247,   250,
   257,   263,   269,   275,   281,   287,   293,   300,   307,   314,
   321,   328,   335,   342,   351,   360,   368,   376,   383,   391,
   397,   403,   412,   419,   428,   433
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAN","IN",
"TO","IDENT","REAL","AVG","PROD","SUM","LET","NEG","LEN","MAX","MIN","ISNAN",
"SQRT","ABS","EXP","LOG","SIN","COS","CLAMP","SEGMENT","LT","LE","GT","GE","EQ",
"NE","NOT","AND","OR","IF","ELSE","FOR","'+'","'-'","'*'","'/'","'('","')'",
"'['","']'","'.'","'='","'^'","'{'","'}'","','","'|'","';'","':'","'?'","top",
"exprlist","expr","letexpr","vector", NULL
};
#endif

static const short yyr1[] = {     0,
    55,    56,    56,    56,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    58,    58,    59,    59
};

static const short yyr2[] = {     0,
     1,     3,     2,     1,     3,     3,     3,     7,     5,     5,
     5,     5,     3,     3,     2,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     4,     3,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     8,     8,     5,     7,     5,     7,     1,
     1,     1,     5,     5,     1,     3
};

static const short yydefact[] = {     0,
    52,    50,    51,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     1,     4,     0,    33,
    32,    31,     0,    29,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,     0,     0,    30,     0,     0,    15,
     0,    55,     0,    50,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     3,
     0,    28,     0,     0,     0,     0,     0,     5,     0,     0,
     7,     0,     0,     6,    19,    20,    21,    22,    23,    24,
    25,    26,    13,    14,    16,    17,     0,    18,     2,     0,
     0,     0,     0,     0,     0,     0,     0,    56,     0,    27,
     0,     0,     0,     0,     0,    48,     0,     9,    10,    11,
    12,     0,    46,    54,    53,     0,     0,     0,     0,     0,
     0,     0,    47,    49,     8,    44,    45,     0,     0,     0
};

static const short yydefgoto[] = {   138,
    27,    28,    34,    53
};

static const short yypact[] = {   150,
-32768,   -15,-32768,   150,   150,   150,    27,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,    -6,    -3,   150,
    -2,    -8,   150,   150,   150,   193,-32768,   293,   150,     0,
     0,     0,    -9,-32768,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   150,   150,   -13,   150,    39,   -13,
   217,   323,   -27,    20,    -1,   150,   150,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
   150,   708,   150,   353,   383,   413,    42,-32768,   150,   150,
-32768,   150,   150,-32768,   -11,   -11,   -11,   -11,    45,    45,
   754,   731,   -25,   -25,   -13,   -13,   436,   -13,-32768,   459,
   101,   150,   150,   150,   150,   247,   270,   708,   489,-32768,
   150,   150,    27,   519,   549,   579,   602,-32768,-32768,-32768,
-32768,   150,   708,   708,-32768,   150,   150,   150,   150,   632,
   662,   685,   708,   708,-32768,-32768,-32768,    49,    50,-32768
};

static const short yypgoto[] = {-32768,
   -23,    -4,   -62,-32768
};


#define	YYLAST		801


static const short yytable[] = {    30,
    31,    32,    55,    35,    36,    37,    38,    39,    40,    41,
    42,    43,    44,    66,    67,    47,    81,    68,    50,    51,
    52,    69,    82,    83,    72,    64,    65,    66,    67,    68,
    29,    68,    33,    69,    45,    69,    73,    46,    48,    49,
    74,    75,    68,    76,    77,   105,    99,    84,   139,   140,
   125,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,    97,    98,    29,   100,     0,   101,    56,
    57,    58,    59,     0,   106,   107,     0,   108,   109,     0,
     0,    64,    65,    66,    67,     0,     0,    68,     0,     0,
     0,    69,     0,     0,     0,     0,     0,   114,   115,   116,
   117,     0,     0,     0,   112,     0,   123,   124,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   130,     0,     0,
     0,   131,   132,   133,   134,    56,    57,    58,    59,    60,
    61,     0,    62,    63,     0,     0,     0,    64,    65,    66,
    67,     0,     0,    68,     0,     0,     0,    69,     0,     0,
   113,     0,     1,     0,    71,     2,     3,     4,     5,     6,
     7,     0,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,     0,     0,     0,     0,     0,     0,
    20,     0,     0,    21,     0,    22,     0,    23,     0,     0,
    24,     0,    25,     0,     0,     1,     0,    26,    54,     3,
     4,     5,     6,     7,     0,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
     0,     0,     0,    20,     0,     0,    21,     0,    22,     0,
    23,     0,     0,    24,     0,    25,     0,     0,     0,     0,
    26,    56,    57,    58,    59,    60,    61,     0,    62,    63,
     0,     0,     0,    64,    65,    66,    67,     0,    78,    68,
     0,     0,     0,    69,     0,     0,     0,     0,     0,    79,
    71,    56,    57,    58,    59,    60,    61,     0,    62,    63,
     0,     0,     0,    64,    65,    66,    67,     0,   118,    68,
   119,     0,     0,    69,    56,    57,    58,    59,    60,    61,
    71,    62,    63,     0,     0,     0,    64,    65,    66,    67,
     0,   120,    68,   121,     0,     0,    69,    56,    57,    58,
    59,    60,    61,    71,    62,    63,     0,     0,     0,    64,
    65,    66,    67,     0,     0,    68,     0,     0,     0,    69,
     0,     0,     0,     0,    70,     0,    71,    56,    57,    58,
    59,    60,    61,     0,    62,    63,     0,     0,     0,    64,
    65,    66,    67,     0,     0,    68,     0,     0,     0,    69,
     0,     0,     0,     0,     0,    80,    71,    56,    57,    58,
    59,    60,    61,     0,    62,    63,     0,     0,     0,    64,
    65,    66,    67,     0,     0,    68,     0,     0,     0,    69,
     0,     0,   102,     0,     0,     0,    71,    56,    57,    58,
    59,    60,    61,     0,    62,    63,     0,     0,     0,    64,
    65,    66,    67,     0,     0,    68,     0,     0,     0,    69,
     0,     0,   103,     0,     0,     0,    71,    56,    57,    58,
    59,    60,    61,     0,    62,    63,     0,     0,     0,    64,
    65,    66,    67,     0,   104,    68,     0,     0,     0,    69,
    56,    57,    58,    59,    60,    61,    71,    62,    63,     0,
     0,     0,    64,    65,    66,    67,     0,     0,    68,   110,
     0,     0,    69,    56,    57,    58,    59,    60,    61,    71,
    62,    63,     0,     0,     0,    64,    65,    66,    67,     0,
     0,    68,     0,     0,     0,    69,     0,     0,     0,     0,
     0,   111,    71,    56,    57,    58,    59,    60,    61,     0,
    62,    63,     0,     0,     0,    64,    65,    66,    67,     0,
     0,    68,     0,     0,     0,    69,     0,     0,     0,   122,
     0,     0,    71,    56,    57,    58,    59,    60,    61,     0,
    62,    63,     0,     0,     0,    64,    65,    66,    67,     0,
     0,    68,     0,     0,     0,    69,     0,     0,   126,     0,
     0,     0,    71,    56,    57,    58,    59,    60,    61,     0,
    62,    63,     0,     0,     0,    64,    65,    66,    67,     0,
     0,    68,     0,     0,     0,    69,     0,     0,   127,     0,
     0,     0,    71,    56,    57,    58,    59,    60,    61,     0,
    62,    63,     0,   128,     0,    64,    65,    66,    67,     0,
     0,    68,     0,     0,     0,    69,    56,    57,    58,    59,
    60,    61,    71,    62,    63,     0,     0,     0,    64,    65,
    66,    67,     0,     0,    68,     0,     0,     0,    69,     0,
   129,     0,     0,     0,     0,    71,    56,    57,    58,    59,
    60,    61,     0,    62,    63,     0,     0,     0,    64,    65,
    66,    67,     0,     0,    68,     0,     0,     0,    69,     0,
   135,     0,     0,     0,     0,    71,    56,    57,    58,    59,
    60,    61,     0,    62,    63,     0,     0,     0,    64,    65,
    66,    67,     0,   136,    68,     0,     0,     0,    69,    56,
    57,    58,    59,    60,    61,    71,    62,    63,     0,     0,
     0,    64,    65,    66,    67,     0,   137,    68,     0,     0,
     0,    69,    56,    57,    58,    59,    60,    61,    71,    62,
    63,     0,     0,     0,    64,    65,    66,    67,     0,     0,
    68,     0,     0,     0,    69,    56,    57,    58,    59,    60,
    61,    71,    62,     0,     0,     0,     0,    64,    65,    66,
    67,     0,     0,    68,     0,     0,     0,    69,    56,    57,
    58,    59,    60,    61,     0,     0,     0,     0,     0,     0,
    64,    65,    66,    67,     0,     0,    68,     0,     0,     0,
    69
};

static const short yycheck[] = {     4,
     5,     6,    26,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    39,    40,    20,    44,    43,    23,    24,
    25,    47,    50,     4,    29,    37,    38,    39,    40,    43,
    46,    43,     6,    47,    41,    47,    46,    41,    41,    48,
    45,    46,    43,    48,     6,     4,    70,    49,     0,     0,
   113,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    46,    71,    -1,    73,    25,
    26,    27,    28,    -1,    79,    80,    -1,    82,    83,    -1,
    -1,    37,    38,    39,    40,    -1,    -1,    43,    -1,    -1,
    -1,    47,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,
   105,    -1,    -1,    -1,     4,    -1,   111,   112,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,    -1,
    -1,   126,   127,   128,   129,    25,    26,    27,    28,    29,
    30,    -1,    32,    33,    -1,    -1,    -1,    37,    38,    39,
    40,    -1,    -1,    43,    -1,    -1,    -1,    47,    -1,    -1,
    50,    -1,     3,    -1,    54,     6,     7,     8,     9,    10,
    11,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,
    31,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
    41,    -1,    43,    -1,    -1,     3,    -1,    48,     6,     7,
     8,     9,    10,    11,    -1,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
    -1,    -1,    -1,    31,    -1,    -1,    34,    -1,    36,    -1,
    38,    -1,    -1,    41,    -1,    43,    -1,    -1,    -1,    -1,
    48,    25,    26,    27,    28,    29,    30,    -1,    32,    33,
    -1,    -1,    -1,    37,    38,    39,    40,    -1,    42,    43,
    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
    54,    25,    26,    27,    28,    29,    30,    -1,    32,    33,
    -1,    -1,    -1,    37,    38,    39,    40,    -1,    42,    43,
    44,    -1,    -1,    47,    25,    26,    27,    28,    29,    30,
    54,    32,    33,    -1,    -1,    -1,    37,    38,    39,    40,
    -1,    42,    43,    44,    -1,    -1,    47,    25,    26,    27,
    28,    29,    30,    54,    32,    33,    -1,    -1,    -1,    37,
    38,    39,    40,    -1,    -1,    43,    -1,    -1,    -1,    47,
    -1,    -1,    -1,    -1,    52,    -1,    54,    25,    26,    27,
    28,    29,    30,    -1,    32,    33,    -1,    -1,    -1,    37,
    38,    39,    40,    -1,    -1,    43,    -1,    -1,    -1,    47,
    -1,    -1,    -1,    -1,    -1,    53,    54,    25,    26,    27,
    28,    29,    30,    -1,    32,    33,    -1,    -1,    -1,    37,
    38,    39,    40,    -1,    -1,    43,    -1,    -1,    -1,    47,
    -1,    -1,    50,    -1,    -1,    -1,    54,    25,    26,    27,
    28,    29,    30,    -1,    32,    33,    -1,    -1,    -1,    37,
    38,    39,    40,    -1,    -1,    43,    -1,    -1,    -1,    47,
    -1,    -1,    50,    -1,    -1,    -1,    54,    25,    26,    27,
    28,    29,    30,    -1,    32,    33,    -1,    -1,    -1,    37,
    38,    39,    40,    -1,    42,    43,    -1,    -1,    -1,    47,
    25,    26,    27,    28,    29,    30,    54,    32,    33,    -1,
    -1,    -1,    37,    38,    39,    40,    -1,    -1,    43,    44,
    -1,    -1,    47,    25,    26,    27,    28,    29,    30,    54,
    32,    33,    -1,    -1,    -1,    37,    38,    39,    40,    -1,
    -1,    43,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
    -1,    53,    54,    25,    26,    27,    28,    29,    30,    -1,
    32,    33,    -1,    -1,    -1,    37,    38,    39,    40,    -1,
    -1,    43,    -1,    -1,    -1,    47,    -1,    -1,    -1,    51,
    -1,    -1,    54,    25,    26,    27,    28,    29,    30,    -1,
    32,    33,    -1,    -1,    -1,    37,    38,    39,    40,    -1,
    -1,    43,    -1,    -1,    -1,    47,    -1,    -1,    50,    -1,
    -1,    -1,    54,    25,    26,    27,    28,    29,    30,    -1,
    32,    33,    -1,    -1,    -1,    37,    38,    39,    40,    -1,
    -1,    43,    -1,    -1,    -1,    47,    -1,    -1,    50,    -1,
    -1,    -1,    54,    25,    26,    27,    28,    29,    30,    -1,
    32,    33,    -1,    35,    -1,    37,    38,    39,    40,    -1,
    -1,    43,    -1,    -1,    -1,    47,    25,    26,    27,    28,
    29,    30,    54,    32,    33,    -1,    -1,    -1,    37,    38,
    39,    40,    -1,    -1,    43,    -1,    -1,    -1,    47,    -1,
    49,    -1,    -1,    -1,    -1,    54,    25,    26,    27,    28,
    29,    30,    -1,    32,    33,    -1,    -1,    -1,    37,    38,
    39,    40,    -1,    -1,    43,    -1,    -1,    -1,    47,    -1,
    49,    -1,    -1,    -1,    -1,    54,    25,    26,    27,    28,
    29,    30,    -1,    32,    33,    -1,    -1,    -1,    37,    38,
    39,    40,    -1,    42,    43,    -1,    -1,    -1,    47,    25,
    26,    27,    28,    29,    30,    54,    32,    33,    -1,    -1,
    -1,    37,    38,    39,    40,    -1,    42,    43,    -1,    -1,
    -1,    47,    25,    26,    27,    28,    29,    30,    54,    32,
    33,    -1,    -1,    -1,    37,    38,    39,    40,    -1,    -1,
    43,    -1,    -1,    -1,    47,    25,    26,    27,    28,    29,
    30,    54,    32,    -1,    -1,    -1,    -1,    37,    38,    39,
    40,    -1,    -1,    43,    -1,    -1,    -1,    47,    25,    26,
    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
    37,    38,    39,    40,    -1,    -1,    43,    -1,    -1,    -1,
    47
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
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_CLAMP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 45:
#line 352 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_SEGMENT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 46:
#line 361 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; ;
    break;}
case 47:
#line 369 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; ;
    break;}
case 48:
#line 377 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->pos = yyvsp[-4].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 49:
#line 384 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_FOR;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 50:
#line 392 "gram.y"
{ yyval.node = new_node(0, ident_is_scalar(yyvsp[0].ident));
        yyval.node->type = NODETYPE_IDENT;
        yyval.node->pos = -1;
        yyval.node->ident = yyvsp[0].ident; ;
    break;}
case 51:
#line 398 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = yyvsp[0].real; ;
    break;}
case 52:
#line 404 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = INVALID_VALUE; ;
    break;}
case 53:
#line 413 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LET;
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 54:
#line 420 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_LET;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 55:
#line 429 "gram.y"
{ yyval.node = new_vector_node(1);
        yyval.node->pos = yyvsp[0].node->pos;
        yyval.node->type = NODETYPE_VEC1;
        yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 56:
#line 434 "gram.y"
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
#line 441 "gram.y"


node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}
