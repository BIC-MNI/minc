/* A Bison parser, made from gram.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	NAN	257
# define	IN	258
# define	TO	259
# define	IDENT	260
# define	REAL	261
# define	AVG	262
# define	PROD	263
# define	SUM	264
# define	LET	265
# define	NEG	266
# define	LEN	267
# define	MAX	268
# define	MIN	269
# define	ISNAN	270
# define	SQRT	271
# define	ABS	272
# define	EXP	273
# define	LOG	274
# define	SIN	275
# define	COS	276
# define	TAN	277
# define	ASIN	278
# define	ACOS	279
# define	ATAN	280
# define	CLAMP	281
# define	SEGMENT	282
# define	LT	283
# define	LE	284
# define	GT	285
# define	GE	286
# define	EQ	287
# define	NE	288
# define	NOT	289
# define	AND	290
# define	OR	291
# define	IF	292
# define	ELSE	293
# define	FOR	294

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
#ifndef YYSTYPE
typedef union{
int      pos;
node_t   node;
double   real;
ident_t  ident;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		148
#define	YYFLAG		-32768
#define	YYNTBASE	59

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 294 ? yytranslate[x] : 64)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      45,    46,    43,    41,    54,    42,    49,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    57,    56,
       2,    50,     2,    58,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,    48,    51,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,    55,    53,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,     9,    11,    15,    19,    23,    31,
      37,    43,    49,    55,    59,    63,    66,    70,    74,    78,
      82,    86,    90,    94,    98,   102,   106,   110,   115,   119,
     122,   125,   128,   131,   134,   137,   140,   143,   146,   149,
     152,   155,   158,   161,   164,   167,   170,   173,   176,   185,
     194,   200,   208,   214,   222,   224,   226,   228,   234,   240,
     242
};
static const short yyrhs[] =
{
      60,     0,    61,    56,    60,     0,    61,    56,     0,    61,
       0,    45,    61,    46,     0,    52,    60,    53,     0,    47,
      63,    48,     0,    52,     6,     4,    61,    55,    61,    53,
       0,    45,    61,    57,    61,    46,     0,    45,    61,    57,
      61,    48,     0,    47,    61,    57,    61,    46,     0,    47,
      61,    57,    61,    48,     0,    61,    41,    61,     0,    61,
      42,    61,     0,    42,    61,     0,    61,    43,    61,     0,
      61,    44,    61,     0,    61,    51,    61,     0,    61,    29,
      61,     0,    61,    30,    61,     0,    61,    31,    61,     0,
      61,    32,    61,     0,    61,    33,    61,     0,    61,    34,
      61,     0,    61,    36,    61,     0,    61,    37,    61,     0,
      61,    47,    61,    48,     0,     6,    50,    61,     0,    11,
      62,     0,    35,    61,     0,    10,    61,     0,     9,    61,
       0,     8,    61,     0,    13,    61,     0,    14,    61,     0,
      15,    61,     0,    16,    61,     0,    17,    61,     0,    18,
      61,     0,    19,    61,     0,    20,    61,     0,    21,    61,
       0,    22,    61,     0,    23,    61,     0,    24,    61,     0,
      25,    61,     0,    26,    61,     0,    27,    45,    61,    54,
      61,    54,    61,    46,     0,    28,    45,    61,    54,    61,
      54,    61,    46,     0,    61,    58,    61,    57,    61,     0,
      38,    45,    61,    46,    61,    39,    61,     0,    38,    45,
      61,    46,    61,     0,    40,    52,     6,     4,    61,    53,
      61,     0,     6,     0,     7,     0,     3,     0,     6,    50,
      61,    54,    62,     0,     6,    50,    61,     4,    61,     0,
      61,     0,    63,    54,    61,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    53,    57,    64,    67,    70,    73,    76,    79,    87,
      95,   103,   111,   119,   127,   135,   145,   153,   161,   169,
     177,   185,   193,   201,   209,   217,   225,   233,   240,   247,
     250,   257,   263,   269,   275,   281,   287,   293,   300,   307,
     314,   321,   328,   335,   342,   349,   356,   363,   370,   379,
     388,   396,   404,   411,   419,   425,   431,   440,   447,   456,
     461
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "NAN", "IN", "TO", "IDENT", "REAL", "AVG", 
  "PROD", "SUM", "LET", "NEG", "LEN", "MAX", "MIN", "ISNAN", "SQRT", 
  "ABS", "EXP", "LOG", "SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN", 
  "CLAMP", "SEGMENT", "LT", "LE", "GT", "GE", "EQ", "NE", "NOT", "AND", 
  "OR", "IF", "ELSE", "FOR", "'+'", "'-'", "'*'", "'/'", "'('", "')'", 
  "'['", "']'", "'.'", "'='", "'^'", "'{'", "'}'", "','", "'|'", "';'", 
  "':'", "'?'", "top", "exprlist", "expr", "letexpr", "vector", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    59,    60,    60,    60,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    62,    62,    63,
      63
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     2,     1,     3,     3,     3,     7,     5,
       5,     5,     5,     3,     3,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     4,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     8,     8,
       5,     7,     5,     7,     1,     1,     1,     5,     5,     1,
       3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,    56,    54,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     1,     4,     0,    33,    32,    31,     0,    29,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,     0,     0,    30,     0,     0,    15,     0,
      59,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,     0,
      28,     0,     0,     0,     0,     0,     5,     0,     0,     7,
       0,     0,     6,    19,    20,    21,    22,    23,    24,    25,
      26,    13,    14,    16,    17,     0,    18,     2,     0,     0,
       0,     0,     0,     0,     0,     0,    60,     0,    27,     0,
       0,     0,     0,     0,    52,     0,     9,    10,    11,    12,
       0,    50,    58,    57,     0,     0,     0,     0,     0,     0,
       0,    51,    53,     8,    48,    49,     0,     0,     0
};

static const short yydefgoto[] =
{
     146,    31,    32,    38,    61
};

static const short yypact[] =
{
     159,-32768,   -29,-32768,   159,   159,   159,    21,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,    -8,    -6,   159,    -2,   -18,   159,   159,   159,
     206,-32768,   306,   159,    -3,    -3,    -3,    -5,-32768,    -3,
      -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,
      -3,    -3,    -3,   159,   159,    -9,   159,    40,    -9,   230,
     336,   -13,    24,     0,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     721,   159,   366,   396,   426,    43,-32768,   159,   159,-32768,
     159,   159,-32768,   -11,   -11,   -11,   -11,   767,   767,    70,
     744,   -25,   -25,    -9,    -9,   449,    -9,-32768,   472,   106,
     159,   159,   159,   159,   260,   283,   721,   502,-32768,   159,
     159,    21,   532,   562,   592,   615,-32768,-32768,-32768,-32768,
     159,   721,   721,-32768,   159,   159,   159,   159,   645,   675,
     698,   721,   721,-32768,-32768,-32768,    48,    54,-32768
};

static const short yypgoto[] =
{
  -32768,   -27,    -4,   -66,-32768
};


#define	YYLAST		818


static const short yytable[] =
{
      34,    35,    36,    63,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    74,    75,
      55,    33,    76,    58,    59,    60,    77,    37,    91,    80,
      72,    73,    74,    75,    57,    89,    76,    53,    76,    54,
      77,    90,    77,    56,    76,    81,    85,   113,   147,    82,
      83,   107,    84,    92,   148,   133,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,    33,   108,     0,   109,     0,     0,
       0,     0,     0,   114,   115,     0,   116,   117,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    64,
      65,    66,    67,    68,    69,     0,   122,   123,   124,   125,
     120,    72,    73,    74,    75,   131,   132,    76,     0,     0,
       0,    77,     0,     0,     0,     0,   138,     0,     0,     0,
     139,   140,   141,   142,     0,    64,    65,    66,    67,    68,
      69,     0,    70,    71,     0,     0,     0,    72,    73,    74,
      75,     0,     0,    76,     0,     0,     0,    77,     0,     0,
     121,     0,     1,     0,    79,     2,     3,     4,     5,     6,
       7,     0,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,     0,     0,
       0,     0,     0,     0,    24,     0,     0,    25,     0,    26,
       0,    27,     0,     0,    28,     0,    29,     0,     0,     1,
       0,    30,    62,     3,     4,     5,     6,     7,     0,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,     0,     0,     0,     0,     0,
       0,    24,     0,     0,    25,     0,    26,     0,    27,     0,
       0,    28,     0,    29,     0,     0,     0,     0,    30,    64,
      65,    66,    67,    68,    69,     0,    70,    71,     0,     0,
       0,    72,    73,    74,    75,     0,    86,    76,     0,     0,
       0,    77,     0,     0,     0,     0,     0,    87,    79,    64,
      65,    66,    67,    68,    69,     0,    70,    71,     0,     0,
       0,    72,    73,    74,    75,     0,   126,    76,   127,     0,
       0,    77,    64,    65,    66,    67,    68,    69,    79,    70,
      71,     0,     0,     0,    72,    73,    74,    75,     0,   128,
      76,   129,     0,     0,    77,    64,    65,    66,    67,    68,
      69,    79,    70,    71,     0,     0,     0,    72,    73,    74,
      75,     0,     0,    76,     0,     0,     0,    77,     0,     0,
       0,     0,    78,     0,    79,    64,    65,    66,    67,    68,
      69,     0,    70,    71,     0,     0,     0,    72,    73,    74,
      75,     0,     0,    76,     0,     0,     0,    77,     0,     0,
       0,     0,     0,    88,    79,    64,    65,    66,    67,    68,
      69,     0,    70,    71,     0,     0,     0,    72,    73,    74,
      75,     0,     0,    76,     0,     0,     0,    77,     0,     0,
     110,     0,     0,     0,    79,    64,    65,    66,    67,    68,
      69,     0,    70,    71,     0,     0,     0,    72,    73,    74,
      75,     0,     0,    76,     0,     0,     0,    77,     0,     0,
     111,     0,     0,     0,    79,    64,    65,    66,    67,    68,
      69,     0,    70,    71,     0,     0,     0,    72,    73,    74,
      75,     0,   112,    76,     0,     0,     0,    77,    64,    65,
      66,    67,    68,    69,    79,    70,    71,     0,     0,     0,
      72,    73,    74,    75,     0,     0,    76,   118,     0,     0,
      77,    64,    65,    66,    67,    68,    69,    79,    70,    71,
       0,     0,     0,    72,    73,    74,    75,     0,     0,    76,
       0,     0,     0,    77,     0,     0,     0,     0,     0,   119,
      79,    64,    65,    66,    67,    68,    69,     0,    70,    71,
       0,     0,     0,    72,    73,    74,    75,     0,     0,    76,
       0,     0,     0,    77,     0,     0,     0,   130,     0,     0,
      79,    64,    65,    66,    67,    68,    69,     0,    70,    71,
       0,     0,     0,    72,    73,    74,    75,     0,     0,    76,
       0,     0,     0,    77,     0,     0,   134,     0,     0,     0,
      79,    64,    65,    66,    67,    68,    69,     0,    70,    71,
       0,     0,     0,    72,    73,    74,    75,     0,     0,    76,
       0,     0,     0,    77,     0,     0,   135,     0,     0,     0,
      79,    64,    65,    66,    67,    68,    69,     0,    70,    71,
       0,   136,     0,    72,    73,    74,    75,     0,     0,    76,
       0,     0,     0,    77,    64,    65,    66,    67,    68,    69,
      79,    70,    71,     0,     0,     0,    72,    73,    74,    75,
       0,     0,    76,     0,     0,     0,    77,     0,   137,     0,
       0,     0,     0,    79,    64,    65,    66,    67,    68,    69,
       0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
       0,     0,    76,     0,     0,     0,    77,     0,   143,     0,
       0,     0,     0,    79,    64,    65,    66,    67,    68,    69,
       0,    70,    71,     0,     0,     0,    72,    73,    74,    75,
       0,   144,    76,     0,     0,     0,    77,    64,    65,    66,
      67,    68,    69,    79,    70,    71,     0,     0,     0,    72,
      73,    74,    75,     0,   145,    76,     0,     0,     0,    77,
      64,    65,    66,    67,    68,    69,    79,    70,    71,     0,
       0,     0,    72,    73,    74,    75,     0,     0,    76,     0,
       0,     0,    77,    64,    65,    66,    67,    68,    69,    79,
      70,     0,     0,     0,     0,    72,    73,    74,    75,     0,
       0,    76,     0,     0,     0,    77,    64,    65,    66,    67,
       0,     0,     0,     0,     0,     0,     0,     0,    72,    73,
      74,    75,     0,     0,    76,     0,     0,     0,    77
};

static const short yycheck[] =
{
       4,     5,     6,    30,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    43,    44,
      24,    50,    47,    27,    28,    29,    51,     6,     4,    33,
      41,    42,    43,    44,    52,    48,    47,    45,    47,    45,
      51,    54,    51,    45,    47,    50,     6,     4,     0,    53,
      54,    78,    56,    53,     0,   121,    -1,    -1,    -1,    -1,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    50,    79,    -1,    81,    -1,    -1,
      -1,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    -1,   110,   111,   112,   113,
       4,    41,    42,    43,    44,   119,   120,    47,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,
     134,   135,   136,   137,    -1,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    -1,     3,    -1,    58,     6,     7,     8,     9,    10,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,    -1,    -1,    -1,    35,    -1,    -1,    38,    -1,    40,
      -1,    42,    -1,    -1,    45,    -1,    47,    -1,    -1,     3,
      -1,    52,     6,     7,     8,     9,    10,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    -1,    -1,    38,    -1,    40,    -1,    42,    -1,
      -1,    45,    -1,    47,    -1,    -1,    -1,    -1,    52,    29,
      30,    31,    32,    33,    34,    -1,    36,    37,    -1,    -1,
      -1,    41,    42,    43,    44,    -1,    46,    47,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    57,    58,    29,
      30,    31,    32,    33,    34,    -1,    36,    37,    -1,    -1,
      -1,    41,    42,    43,    44,    -1,    46,    47,    48,    -1,
      -1,    51,    29,    30,    31,    32,    33,    34,    58,    36,
      37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,    46,
      47,    48,    -1,    -1,    51,    29,    30,    31,    32,    33,
      34,    58,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      -1,    -1,    56,    -1,    58,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      -1,    -1,    -1,    57,    58,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    -1,    -1,    -1,    58,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    -1,    -1,    -1,    58,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    46,    47,    -1,    -1,    -1,    51,    29,    30,
      31,    32,    33,    34,    58,    36,    37,    -1,    -1,    -1,
      41,    42,    43,    44,    -1,    -1,    47,    48,    -1,    -1,
      51,    29,    30,    31,    32,    33,    34,    58,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,
      -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    -1,    57,
      58,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,
      -1,    -1,    -1,    51,    -1,    -1,    -1,    55,    -1,    -1,
      58,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,
      -1,    -1,    -1,    51,    -1,    -1,    54,    -1,    -1,    -1,
      58,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,
      -1,    -1,    -1,    51,    -1,    -1,    54,    -1,    -1,    -1,
      58,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    39,    -1,    41,    42,    43,    44,    -1,    -1,    47,
      -1,    -1,    -1,    51,    29,    30,    31,    32,    33,    34,
      58,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
      -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    53,    -1,
      -1,    -1,    -1,    58,    29,    30,    31,    32,    33,    34,
      -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
      -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    53,    -1,
      -1,    -1,    -1,    58,    29,    30,    31,    32,    33,    34,
      -1,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
      -1,    46,    47,    -1,    -1,    -1,    51,    29,    30,    31,
      32,    33,    34,    58,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    46,    47,    -1,    -1,    -1,    51,
      29,    30,    31,    32,    33,    34,    58,    36,    37,    -1,
      -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,    -1,
      -1,    -1,    51,    29,    30,    31,    32,    33,    34,    58,
      36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    -1,    -1,    -1,    51,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
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
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 54 "gram.y"
{ root = yyvsp[0].node; }
    break;
case 2:
#line 58 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_EXPRLIST;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 3:
#line 65 "gram.y"
{ yyval.node = yyvsp[-1].node; }
    break;
case 4:
#line 68 "gram.y"
{ yyval.node = yyvsp[0].node; }
    break;
case 5:
#line 71 "gram.y"
{ yyval.node = yyvsp[-1].node; }
    break;
case 6:
#line 74 "gram.y"
{ yyval.node = yyvsp[-1].node; }
    break;
case 7:
#line 77 "gram.y"
{ yyval.node = yyvsp[-1].node; }
    break;
case 8:
#line 80 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_GEN;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->ident = yyvsp[-5].ident;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;
case 9:
#line 88 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = 0;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;
case 10:
#line 96 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;
case 11:
#line 104 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;
case 12:
#line 112 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;
case 13:
#line 120 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_ADD;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 14:
#line 128 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 15:
#line 136 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = new_scalar_node(0);
        yyval.node->expr[0]->type = NODETYPE_REAL;
        yyval.node->expr[0]->real = 0.0;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 16:
#line 146 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MUL;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 17:
#line 154 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_DIV;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 18:
#line 162 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_POW;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 19:
#line 170 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 20:
#line 178 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 21:
#line 186 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 22:
#line 194 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 23:
#line 202 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_EQ;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 24:
#line 210 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_NE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 25:
#line 218 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_AND;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 26:
#line 226 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_OR;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 27:
#line 234 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_INDEX;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;
case 28:
#line 241 "gram.y"
{ yyval.node = new_node(1, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_ASSIGN;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->ident = yyvsp[-2].ident;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 29:
#line 248 "gram.y"
{ yyval.node = yyvsp[0].node; }
    break;
case 30:
#line 251 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_NOT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 31:
#line 258 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SUM;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 32:
#line 264 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_PROD;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 33:
#line 270 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_AVG;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 34:
#line 276 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LEN;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 35:
#line 282 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MAX;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 36:
#line 288 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MIN;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 37:
#line 294 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ISNAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 38:
#line 301 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SQRT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 39:
#line 308 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ABS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 40:
#line 315 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_EXP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 41:
#line 322 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LOG;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 42:
#line 329 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 43:
#line 336 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_COS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 44:
#line 343 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_TAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 45:
#line 350 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ASIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 46:
#line 357 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ACOS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 47:
#line 364 "gram.y"
{ yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ATAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 48:
#line 371 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_CLAMP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; }
    break;
case 49:
#line 380 "gram.y"
{ yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_SEGMENT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; }
    break;
case 50:
#line 389 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; }
    break;
case 51:
#line 397 "gram.y"
{ yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; }
    break;
case 52:
#line 405 "gram.y"
{ yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->pos = yyvsp[-4].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 53:
#line 412 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_FOR;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 54:
#line 420 "gram.y"
{ yyval.node = new_node(0, ident_is_scalar(yyvsp[0].ident));
        yyval.node->type = NODETYPE_IDENT;
        yyval.node->pos = -1;
        yyval.node->ident = yyvsp[0].ident; }
    break;
case 55:
#line 426 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = yyvsp[0].real; }
    break;
case 56:
#line 432 "gram.y"
{ yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = INVALID_VALUE; }
    break;
case 57:
#line 441 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LET;
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 58:
#line 448 "gram.y"
{ yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_LET;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
case 59:
#line 457 "gram.y"
{ yyval.node = new_vector_node(1);
        yyval.node->pos = yyvsp[0].node->pos;
        yyval.node->type = NODETYPE_VEC1;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;
case 60:
#line 462 "gram.y"
{ yyval.node = new_vector_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_VEC2;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
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
