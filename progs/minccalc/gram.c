
/*  A Bison parser, made from gram.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	IN	257
#define	TO	258
#define	IDENT	259
#define	REAL	260
#define	AVG	261
#define	SUM	262
#define	LET	263
#define	NEG	264
#define	LEN	265
#define	MAX	266
#define	MIN	267
#define	ISNAN	268
#define	SQRT	269
#define	ABS	270
#define	EXP	271
#define	LOG	272
#define	SIN	273
#define	COS	274
#define	CLAMP	275
#define	SEGMENT	276
#define	LT	277
#define	LE	278
#define	GT	279
#define	GE	280
#define	EQ	281
#define	NE	282
#define	NOT	283
#define	AND	284
#define	OR	285

#line 1 "gram.y"

#include <stdio.h>
#include <errno.h>
#include "node.h"

/* Avoid problems with conflicting declarations */
void yyerror(const char *msg);

#line 10 "gram.y"
typedef union{
int		pos;
node_t 		node;
float		real;
ident_t		ident;
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



#define	YYFINAL		113
#define	YYFLAG		-32768
#define	YYNTBASE	47

#define YYTRANSLATE(x) ((unsigned)(x) <= 285 ? yytranslate[x] : 51)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    36,
    37,    34,    32,    45,    33,    40,    35,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    41,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    38,     2,    39,    42,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    43,    46,    44,     2,     2,     2,     2,     2,
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
    27,    28,    29,    30,    31
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,    10,    18,    24,    30,    36,    42,    46,
    50,    53,    57,    61,    65,    69,    73,    77,    81,    85,
    89,    93,    97,   102,   105,   108,   111,   114,   117,   120,
   123,   126,   129,   132,   135,   138,   141,   144,   153,   162,
   164,   166,   172,   178,   180
};

static const short yyrhs[] = {    48,
     0,    36,    48,    37,     0,    43,    50,    44,     0,    43,
     5,     3,    48,    46,    48,    44,     0,    36,    48,    45,
    48,    37,     0,    36,    48,    45,    48,    39,     0,    38,
    48,    45,    48,    37,     0,    38,    48,    45,    48,    39,
     0,    48,    32,    48,     0,    48,    33,    48,     0,    33,
    48,     0,    48,    34,    48,     0,    48,    35,    48,     0,
    48,    42,    48,     0,    48,    23,    48,     0,    48,    24,
    48,     0,    48,    25,    48,     0,    48,    26,    48,     0,
    48,    27,    48,     0,    48,    28,    48,     0,    48,    30,
    48,     0,    48,    31,    48,     0,    48,    38,    48,    39,
     0,     9,    49,     0,    29,    48,     0,     8,    48,     0,
     7,    48,     0,    11,    48,     0,    12,    48,     0,    13,
    48,     0,    14,    48,     0,    15,    48,     0,    16,    48,
     0,    17,    48,     0,    18,    48,     0,    19,    48,     0,
    20,    48,     0,    21,    36,    48,    45,    48,    45,    48,
    37,     0,    22,    36,    48,    45,    48,    45,    48,    37,
     0,     5,     0,     6,     0,     5,    41,    48,    45,    49,
     0,     5,    41,    48,     3,    48,     0,    48,     0,    50,
    45,    48,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    44,    48,    51,    54,    62,    70,    78,    86,    94,   102,
   110,   120,   128,   136,   144,   152,   160,   168,   176,   184,
   192,   200,   208,   215,   218,   225,   231,   237,   243,   249,
   255,   262,   269,   276,   283,   290,   297,   304,   313,   322,
   328,   337,   344,   353,   358
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","IN","TO",
"IDENT","REAL","AVG","SUM","LET","NEG","LEN","MAX","MIN","ISNAN","SQRT","ABS",
"EXP","LOG","SIN","COS","CLAMP","SEGMENT","LT","LE","GT","GE","EQ","NE","NOT",
"AND","OR","'+'","'-'","'*'","'/'","'('","')'","'['","']'","'.'","'='","'^'",
"'{'","'}'","','","'|'","top","expr","letexpr","vector", NULL
};
#endif

static const short yyr1[] = {     0,
    47,    48,    48,    48,    48,    48,    48,    48,    48,    48,
    48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
    48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
    48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
    48,    49,    49,    50,    50
};

static const short yyr2[] = {     0,
     1,     3,     3,     7,     5,     5,     5,     5,     3,     3,
     2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     4,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     8,     8,     1,
     1,     5,     5,     1,     3
};

static const short yydefact[] = {     0,
    40,    41,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     1,    27,    26,     0,    24,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,     0,     0,    25,
    11,     0,     0,    40,    44,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     2,     0,     0,     0,     3,     0,    15,
    16,    17,    18,    19,    20,    21,    22,     9,    10,    12,
    13,     0,    14,     0,     0,     0,     0,     0,     0,    45,
    23,     0,     0,     0,     0,     5,     6,     7,     8,     0,
    43,    42,     0,     0,     0,     0,     0,     4,    38,    39,
     0,     0,     0
};

static const short yydefgoto[] = {   111,
    23,    27,    46
};

static const short yypact[] = {   122,
-32768,-32768,   122,   122,    15,   122,   122,   122,   122,   122,
   122,   122,   122,   122,   122,   -34,   -15,   122,   122,   122,
   122,   161,   444,   -10,   -10,    -2,-32768,   -10,   -10,   -10,
   -10,   -10,   -10,   -10,   -10,   -10,   -10,   122,   122,     0,
     0,   206,   229,    37,   444,   -31,   122,   122,   122,   122,
   122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
   122,   252,   275,-32768,   122,   122,   122,-32768,   122,   114,
   114,   114,   114,   484,   484,    53,   464,    33,    33,     0,
     0,   384,     0,    81,   122,   122,   344,   364,   182,   444,
-32768,   122,    15,   298,   321,-32768,-32768,-32768,-32768,   122,
   444,-32768,   122,   122,    -1,   404,   424,-32768,-32768,-32768,
    59,    60,-32768
};

static const short yypgoto[] = {-32768,
    -3,   -32,-32768
};


#define	YYLAST		526


static const short yytable[] = {    24,
    25,    38,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    68,    69,    40,    41,    42,    43,    45,    26,
    39,    47,    48,    49,    50,    51,    52,    59,    53,    54,
    55,    56,    57,    58,    62,    63,    59,    59,    61,    67,
    60,    60,   108,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,   112,   113,
   102,    87,    88,    89,     0,    90,    57,    58,     0,     0,
    59,     0,     0,     0,    60,    47,    48,    49,    50,    51,
    52,    94,    95,    92,    55,    56,    57,    58,   101,     0,
    59,     0,     0,     0,    60,     0,   105,     0,     0,   106,
   107,     0,     0,    47,    48,    49,    50,    51,    52,     0,
    53,    54,    55,    56,    57,    58,     0,     0,    59,     0,
     0,     0,    60,     0,     0,    93,     1,     2,     3,     4,
     5,     0,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,     0,    55,    56,    57,    58,     0,
    18,    59,     0,     0,    19,    60,     0,    20,     0,    21,
     0,     0,     0,     0,    22,    44,     2,     3,     4,     5,
     0,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,     0,     0,     0,     0,     0,     0,    18,
     0,     0,     0,    19,     0,     0,    20,     0,    21,     0,
     0,     0,     0,    22,    47,    48,    49,    50,    51,    52,
     0,    53,    54,    55,    56,    57,    58,     0,     0,    59,
     0,     0,     0,    60,     0,     0,     0,   100,    47,    48,
    49,    50,    51,    52,     0,    53,    54,    55,    56,    57,
    58,     0,    64,    59,     0,     0,     0,    60,     0,     0,
    65,    47,    48,    49,    50,    51,    52,     0,    53,    54,
    55,    56,    57,    58,     0,     0,    59,     0,     0,     0,
    60,     0,     0,    66,    47,    48,    49,    50,    51,    52,
     0,    53,    54,    55,    56,    57,    58,     0,     0,    59,
     0,     0,     0,    60,     0,     0,    85,    47,    48,    49,
    50,    51,    52,     0,    53,    54,    55,    56,    57,    58,
     0,     0,    59,     0,     0,     0,    60,     0,     0,    86,
    47,    48,    49,    50,    51,    52,     0,    53,    54,    55,
    56,    57,    58,     0,     0,    59,     0,     0,     0,    60,
     0,     0,   103,    47,    48,    49,    50,    51,    52,     0,
    53,    54,    55,    56,    57,    58,     0,     0,    59,     0,
     0,     0,    60,     0,     0,   104,    47,    48,    49,    50,
    51,    52,     0,    53,    54,    55,    56,    57,    58,     0,
    96,    59,    97,     0,     0,    60,    47,    48,    49,    50,
    51,    52,     0,    53,    54,    55,    56,    57,    58,     0,
    98,    59,    99,     0,     0,    60,    47,    48,    49,    50,
    51,    52,     0,    53,    54,    55,    56,    57,    58,     0,
     0,    59,    91,     0,     0,    60,    47,    48,    49,    50,
    51,    52,     0,    53,    54,    55,    56,    57,    58,     0,
   109,    59,     0,     0,     0,    60,    47,    48,    49,    50,
    51,    52,     0,    53,    54,    55,    56,    57,    58,     0,
   110,    59,     0,     0,     0,    60,    47,    48,    49,    50,
    51,    52,     0,    53,    54,    55,    56,    57,    58,     0,
     0,    59,     0,     0,     0,    60,    47,    48,    49,    50,
    51,    52,     0,    53,     0,    55,    56,    57,    58,     0,
     0,    59,     0,     0,     0,    60,    47,    48,    49,    50,
     0,     0,     0,     0,     0,    55,    56,    57,    58,     0,
     0,    59,     0,     0,     0,    60
};

static const short yycheck[] = {     3,
     4,    36,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    44,    45,    18,    19,    20,    21,    22,     5,
    36,    23,    24,    25,    26,    27,    28,    38,    30,    31,
    32,    33,    34,    35,    38,    39,    38,    38,    41,     3,
    42,    42,    44,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,     0,     0,
    93,    65,    66,    67,    -1,    69,    34,    35,    -1,    -1,
    38,    -1,    -1,    -1,    42,    23,    24,    25,    26,    27,
    28,    85,    86,     3,    32,    33,    34,    35,    92,    -1,
    38,    -1,    -1,    -1,    42,    -1,   100,    -1,    -1,   103,
   104,    -1,    -1,    23,    24,    25,    26,    27,    28,    -1,
    30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
    -1,    -1,    42,    -1,    -1,    45,     5,     6,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    -1,    32,    33,    34,    35,    -1,
    29,    38,    -1,    -1,    33,    42,    -1,    36,    -1,    38,
    -1,    -1,    -1,    -1,    43,     5,     6,     7,     8,     9,
    -1,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    29,
    -1,    -1,    -1,    33,    -1,    -1,    36,    -1,    38,    -1,
    -1,    -1,    -1,    43,    23,    24,    25,    26,    27,    28,
    -1,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
    -1,    -1,    -1,    42,    -1,    -1,    -1,    46,    23,    24,
    25,    26,    27,    28,    -1,    30,    31,    32,    33,    34,
    35,    -1,    37,    38,    -1,    -1,    -1,    42,    -1,    -1,
    45,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
    42,    -1,    -1,    45,    23,    24,    25,    26,    27,    28,
    -1,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
    -1,    -1,    -1,    42,    -1,    -1,    45,    23,    24,    25,
    26,    27,    28,    -1,    30,    31,    32,    33,    34,    35,
    -1,    -1,    38,    -1,    -1,    -1,    42,    -1,    -1,    45,
    23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    42,
    -1,    -1,    45,    23,    24,    25,    26,    27,    28,    -1,
    30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
    -1,    -1,    42,    -1,    -1,    45,    23,    24,    25,    26,
    27,    28,    -1,    30,    31,    32,    33,    34,    35,    -1,
    37,    38,    39,    -1,    -1,    42,    23,    24,    25,    26,
    27,    28,    -1,    30,    31,    32,    33,    34,    35,    -1,
    37,    38,    39,    -1,    -1,    42,    23,    24,    25,    26,
    27,    28,    -1,    30,    31,    32,    33,    34,    35,    -1,
    -1,    38,    39,    -1,    -1,    42,    23,    24,    25,    26,
    27,    28,    -1,    30,    31,    32,    33,    34,    35,    -1,
    37,    38,    -1,    -1,    -1,    42,    23,    24,    25,    26,
    27,    28,    -1,    30,    31,    32,    33,    34,    35,    -1,
    37,    38,    -1,    -1,    -1,    42,    23,    24,    25,    26,
    27,    28,    -1,    30,    31,    32,    33,    34,    35,    -1,
    -1,    38,    -1,    -1,    -1,    42,    23,    24,    25,    26,
    27,    28,    -1,    30,    -1,    32,    33,    34,    35,    -1,
    -1,    38,    -1,    -1,    -1,    42,    23,    24,    25,    26,
    -1,    -1,    -1,    -1,    -1,    32,    33,    34,    35,    -1,
    -1,    38,    -1,    -1,    -1,    42
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
#line 45 "gram.y"
{ root = yyvsp[0].node; ;
    break;}
case 2:
#line 49 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 3:
#line 52 "gram.y"
{ yyval.node = yyvsp[-1].node; ;
    break;}
case 4:
#line 55 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_GEN;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->ident = yyvsp[-5].ident;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 5:
#line 63 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = 0;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 6:
#line 71 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_UPPER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 7:
#line 79 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_LOWER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 8:
#line 87 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_RANGE;
		  yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 9:
#line 95 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_ADD;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 10:
#line 103 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_SUB;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 11:
#line 111 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_SUB;
		  yyval.node->pos = yyvsp[-1].pos;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = new_node(0);
		  yyval.node->expr[0]->type = NODETYPE_REAL;
		  yyval.node->expr[0]->real = 0.0;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 12:
#line 121 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_MUL;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 13:
#line 129 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_DIV;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 14:
#line 137 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_POW;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 15:
#line 145 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_LT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 16:
#line 153 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_LE;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 17:
#line 161 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_GT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 18:
#line 169 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_GE;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 19:
#line 177 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_EQ;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 20:
#line 185 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_NE;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 21:
#line 193 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_AND;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 22:
#line 201 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_OR;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 23:
#line 209 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_INDEX;
		  yyval.node->pos = yyvsp[0].pos;
		  yyval.node->expr[0] = yyvsp[-3].node;
		  yyval.node->expr[1] = yyvsp[-1].node; ;
    break;}
case 24:
#line 216 "gram.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 25:
#line 219 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_NOT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 26:
#line 226 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_SUM;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 27:
#line 232 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_AVG;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 28:
#line 238 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_LEN;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 29:
#line 244 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_MAX;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 30:
#line 250 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_MIN;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 31:
#line 256 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_ISNAN;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 32:
#line 263 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_SQRT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 33:
#line 270 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_ABS;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 34:
#line 277 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_EXP;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 35:
#line 284 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_LOG;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 36:
#line 291 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_SIN;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 37:
#line 298 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[-1].pos;
		  yyval.node->type = NODETYPE_COS;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 38:
#line 305 "gram.y"
{ yyval.node = new_node(3);
		  yyval.node->pos = yyvsp[-7].pos;
		  yyval.node->type = NODETYPE_CLAMP;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-5].node;
		  yyval.node->expr[1] = yyvsp[-3].node;
		  yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 39:
#line 314 "gram.y"
{ yyval.node = new_node(3);
		  yyval.node->pos = yyvsp[-7].pos;
		  yyval.node->type = NODETYPE_SEGMENT;
        yyval.node->flags |= ALLARGS_SCALAR;
		  yyval.node->expr[0] = yyvsp[-5].node;
		  yyval.node->expr[1] = yyvsp[-3].node;
		  yyval.node->expr[2] = yyvsp[-1].node; ;
    break;}
case 40:
#line 323 "gram.y"
{ yyval.node = new_node(0);
		  yyval.node->type = NODETYPE_IDENT;
		  yyval.node->pos = -1;
		  yyval.node->ident = yyvsp[0].ident; ;
    break;}
case 41:
#line 329 "gram.y"
{ yyval.node = new_node(0);
		  yyval.node->pos = -1;
		  yyval.node->type = NODETYPE_REAL;
		  yyval.node->real = yyvsp[0].real; ;
    break;}
case 42:
#line 338 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->type = NODETYPE_LET;
		  yyval.node->pos = yyvsp[-3].pos;
		  yyval.node->ident = yyvsp[-4].ident;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 43:
#line 345 "gram.y"
{ yyval.node = new_node(2);
		  yyval.node->pos = yyvsp[-3].pos;
		  yyval.node->type = NODETYPE_LET;
		  yyval.node->ident = yyvsp[-4].ident;
		  yyval.node->expr[0] = yyvsp[-2].node;
		  yyval.node->expr[1] = yyvsp[0].node; ;
    break;}
case 44:
#line 354 "gram.y"
{ yyval.node = new_node(1);
		  yyval.node->pos = yyvsp[0].node->pos;
		  yyval.node->type = NODETYPE_VEC1;
		  yyval.node->expr[0] = yyvsp[0].node; ;
    break;}
case 45:
#line 359 "gram.y"
{ yyval.node = new_node(2);
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
#line 366 "gram.y"


node_t root;

void
yyerror(msg)
	const char *msg;
{
	extern int lexpos;

	show_error(lexpos, msg);
}
