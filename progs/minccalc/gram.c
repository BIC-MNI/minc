/* A Bison parser, made from gram.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NAN = 258,
     IN = 259,
     TO = 260,
     IDENT = 261,
     REAL = 262,
     AVG = 263,
     PROD = 264,
     SUM = 265,
     LET = 266,
     NEG = 267,
     LEN = 268,
     MAX = 269,
     MIN = 270,
     ISNAN = 271,
     SQRT = 272,
     ABS = 273,
     EXP = 274,
     LOG = 275,
     SIN = 276,
     COS = 277,
     TAN = 278,
     ASIN = 279,
     ACOS = 280,
     ATAN = 281,
     CLAMP = 282,
     SEGMENT = 283,
     LT = 284,
     LE = 285,
     GT = 286,
     GE = 287,
     EQ = 288,
     NE = 289,
     NOT = 290,
     AND = 291,
     OR = 292,
     IF = 293,
     ELSE = 294,
     FOR = 295
   };
#endif
#define NAN 258
#define IN 259
#define TO 260
#define IDENT 261
#define REAL 262
#define AVG 263
#define PROD 264
#define SUM 265
#define LET 266
#define NEG 267
#define LEN 268
#define MAX 269
#define MIN 270
#define ISNAN 271
#define SQRT 272
#define ABS 273
#define EXP 274
#define LOG 275
#define SIN 276
#define COS 277
#define TAN 278
#define ASIN 279
#define ACOS 280
#define ATAN 281
#define CLAMP 282
#define SEGMENT 283
#define LT 284
#define LE 285
#define GT 286
#define GE 287
#define EQ 288
#define NE 289
#define NOT 290
#define AND 291
#define OR 292
#define IF 293
#define ELSE 294
#define FOR 295




/* Copy the first part of user declarations.  */
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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
#line 15 "gram.y"
typedef union {
int      pos;
node_t   node;
double   real;
ident_t  ident;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 174 "y.tab.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 195 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

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
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

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
	    (To)[yyi] = (From)[yyi];	\
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

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  65
#define YYLAST   820

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  6
/* YYNRULES -- Number of rules. */
#define YYNRULES  61
/* YYNRULES -- Number of states. */
#define YYNSTATES  148

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    14,    18,    22,    26,
      34,    40,    46,    52,    58,    62,    66,    69,    73,    77,
      81,    85,    89,    93,    97,   101,   105,   109,   113,   118,
     122,   125,   128,   131,   134,   137,   140,   143,   146,   149,
     152,   155,   158,   161,   164,   167,   170,   173,   176,   179,
     188,   197,   203,   211,   217,   225,   227,   229,   231,   237,
     243,   245
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      60,     0,    -1,    61,    -1,    62,    56,    61,    -1,    62,
      56,    -1,    62,    -1,    45,    62,    46,    -1,    52,    61,
      53,    -1,    47,    64,    48,    -1,    52,     6,     4,    62,
      55,    62,    53,    -1,    45,    62,    57,    62,    46,    -1,
      45,    62,    57,    62,    48,    -1,    47,    62,    57,    62,
      46,    -1,    47,    62,    57,    62,    48,    -1,    62,    41,
      62,    -1,    62,    42,    62,    -1,    42,    62,    -1,    62,
      43,    62,    -1,    62,    44,    62,    -1,    62,    51,    62,
      -1,    62,    29,    62,    -1,    62,    30,    62,    -1,    62,
      31,    62,    -1,    62,    32,    62,    -1,    62,    33,    62,
      -1,    62,    34,    62,    -1,    62,    36,    62,    -1,    62,
      37,    62,    -1,    62,    47,    62,    48,    -1,     6,    50,
      62,    -1,    11,    63,    -1,    35,    62,    -1,    10,    62,
      -1,     9,    62,    -1,     8,    62,    -1,    13,    62,    -1,
      14,    62,    -1,    15,    62,    -1,    16,    62,    -1,    17,
      62,    -1,    18,    62,    -1,    19,    62,    -1,    20,    62,
      -1,    21,    62,    -1,    22,    62,    -1,    23,    62,    -1,
      24,    62,    -1,    25,    62,    -1,    26,    62,    -1,    27,
      45,    62,    54,    62,    54,    62,    46,    -1,    28,    45,
      62,    54,    62,    54,    62,    46,    -1,    62,    58,    62,
      57,    62,    -1,    38,    45,    62,    46,    62,    39,    62,
      -1,    38,    45,    62,    46,    62,    -1,    40,    52,     6,
       4,    62,    53,    62,    -1,     6,    -1,     7,    -1,     3,
      -1,     6,    50,    62,    54,    63,    -1,     6,    50,    62,
       4,    62,    -1,    62,    -1,    64,    54,    62,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,    53,    53,    57,    64,    67,    71,    74,    77,    80,
      88,    96,   104,   112,   120,   128,   136,   146,   154,   162,
     170,   178,   186,   194,   202,   210,   218,   226,   234,   241,
     248,   251,   258,   264,   270,   276,   282,   288,   294,   301,
     308,   315,   322,   329,   336,   343,   350,   357,   364,   371,
     380,   389,   397,   405,   412,   420,   426,   432,   441,   448,
     457,   462
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAN", "IN", "TO", "IDENT", "REAL", "AVG", 
  "PROD", "SUM", "LET", "NEG", "LEN", "MAX", "MIN", "ISNAN", "SQRT", 
  "ABS", "EXP", "LOG", "SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN", 
  "CLAMP", "SEGMENT", "LT", "LE", "GT", "GE", "EQ", "NE", "NOT", "AND", 
  "OR", "IF", "ELSE", "FOR", "'+'", "'-'", "'*'", "'/'", "'('", "')'", 
  "'['", "']'", "'.'", "'='", "'^'", "'{'", "'}'", "','", "'|'", "';'", 
  "':'", "'?'", "$accept", "top", "exprlist", "expr", "letexpr", "vector", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,    43,    45,    42,    47,    40,    41,    91,    93,    46,
      61,    94,   123,   125,    44,   124,    59,    58,    63
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    59,    60,    61,    61,    61,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    63,    63,
      64,    64
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     2,     1,     3,     3,     3,     7,
       5,     5,     5,     5,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     4,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     8,
       8,     5,     7,     5,     7,     1,     1,     1,     5,     5,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    57,    55,    56,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     5,     0,    34,    33,    32,     0,    30,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,     0,     0,    31,     0,     0,    16,
       0,    60,     0,    55,     0,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     0,    29,     0,     0,     0,     0,     0,     6,     0,
       0,     8,     0,     0,     7,    20,    21,    22,    23,    24,
      25,    26,    27,    14,    15,    17,    18,     0,    19,     3,
       0,     0,     0,     0,     0,     0,     0,     0,    61,     0,
      28,     0,     0,     0,     0,     0,    53,     0,    10,    11,
      12,    13,     0,    51,    59,    58,     0,     0,     0,     0,
       0,     0,     0,    52,    54,     9,    49,    50
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    31,    32,    33,    39,    62
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -76
static const short yypact[] =
{
     161,   -76,   -21,   -76,   161,   161,   161,    30,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,    -7,    -5,   161,    -3,    -9,   161,   161,   161,
     208,    44,   -76,   308,   161,     0,     0,     0,     2,   -76,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   161,   161,   -12,   161,    39,   -12,
     232,   338,   -27,    -1,     1,   -76,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   723,   161,   368,   398,   428,    42,   -76,   161,
     161,   -76,   161,   161,   -76,   -10,   -10,   -10,   -10,   769,
     769,    72,   746,   -25,   -25,   -12,   -12,   451,   -12,   -76,
     474,   108,   161,   161,   161,   161,   262,   285,   723,   504,
     -76,   161,   161,    30,   534,   564,   594,   617,   -76,   -76,
     -76,   -76,   161,   723,   723,   -76,   161,   161,   161,   161,
     647,   677,   700,   723,   723,   -76,   -76,   -76
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -76,   -76,    -2,    -4,   -75,   -76
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      35,    36,    37,    93,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    76,    77,
      56,    91,    78,    59,    60,    61,    79,    92,    64,    34,
      82,    74,    75,    76,    77,    78,    38,    78,    54,    79,
      55,    79,    57,    58,    65,    87,   115,    78,   135,    34,
      84,    85,    83,    86,    94,     0,     0,     0,     0,     0,
       0,     0,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,     0,   110,   109,   111,
       0,     0,     0,     0,     0,   116,   117,     0,   118,   119,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    66,    67,    68,    69,    70,    71,     0,   124,   125,
     126,   127,   122,    74,    75,    76,    77,   133,   134,    78,
       0,     0,     0,    79,     0,     0,     0,     0,   140,     0,
       0,     0,   141,   142,   143,   144,     0,    66,    67,    68,
      69,    70,    71,     0,    72,    73,     0,     0,     0,    74,
      75,    76,    77,     0,     0,    78,     0,     0,     0,    79,
       0,     0,   123,     0,     1,     0,    81,     2,     3,     4,
       5,     6,     7,     0,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
       0,     0,     0,     0,     0,     0,    24,     0,     0,    25,
       0,    26,     0,    27,     0,     0,    28,     0,    29,     0,
       0,     1,     0,    30,    63,     3,     4,     5,     6,     7,
       0,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,     0,     0,     0,
       0,     0,     0,    24,     0,     0,    25,     0,    26,     0,
      27,     0,     0,    28,     0,    29,     0,     0,     0,     0,
      30,    66,    67,    68,    69,    70,    71,     0,    72,    73,
       0,     0,     0,    74,    75,    76,    77,     0,    88,    78,
       0,     0,     0,    79,     0,     0,     0,     0,     0,    89,
      81,    66,    67,    68,    69,    70,    71,     0,    72,    73,
       0,     0,     0,    74,    75,    76,    77,     0,   128,    78,
     129,     0,     0,    79,    66,    67,    68,    69,    70,    71,
      81,    72,    73,     0,     0,     0,    74,    75,    76,    77,
       0,   130,    78,   131,     0,     0,    79,    66,    67,    68,
      69,    70,    71,    81,    72,    73,     0,     0,     0,    74,
      75,    76,    77,     0,     0,    78,     0,     0,     0,    79,
       0,     0,     0,     0,    80,     0,    81,    66,    67,    68,
      69,    70,    71,     0,    72,    73,     0,     0,     0,    74,
      75,    76,    77,     0,     0,    78,     0,     0,     0,    79,
       0,     0,     0,     0,     0,    90,    81,    66,    67,    68,
      69,    70,    71,     0,    72,    73,     0,     0,     0,    74,
      75,    76,    77,     0,     0,    78,     0,     0,     0,    79,
       0,     0,   112,     0,     0,     0,    81,    66,    67,    68,
      69,    70,    71,     0,    72,    73,     0,     0,     0,    74,
      75,    76,    77,     0,     0,    78,     0,     0,     0,    79,
       0,     0,   113,     0,     0,     0,    81,    66,    67,    68,
      69,    70,    71,     0,    72,    73,     0,     0,     0,    74,
      75,    76,    77,     0,   114,    78,     0,     0,     0,    79,
      66,    67,    68,    69,    70,    71,    81,    72,    73,     0,
       0,     0,    74,    75,    76,    77,     0,     0,    78,   120,
       0,     0,    79,    66,    67,    68,    69,    70,    71,    81,
      72,    73,     0,     0,     0,    74,    75,    76,    77,     0,
       0,    78,     0,     0,     0,    79,     0,     0,     0,     0,
       0,   121,    81,    66,    67,    68,    69,    70,    71,     0,
      72,    73,     0,     0,     0,    74,    75,    76,    77,     0,
       0,    78,     0,     0,     0,    79,     0,     0,     0,   132,
       0,     0,    81,    66,    67,    68,    69,    70,    71,     0,
      72,    73,     0,     0,     0,    74,    75,    76,    77,     0,
       0,    78,     0,     0,     0,    79,     0,     0,   136,     0,
       0,     0,    81,    66,    67,    68,    69,    70,    71,     0,
      72,    73,     0,     0,     0,    74,    75,    76,    77,     0,
       0,    78,     0,     0,     0,    79,     0,     0,   137,     0,
       0,     0,    81,    66,    67,    68,    69,    70,    71,     0,
      72,    73,     0,   138,     0,    74,    75,    76,    77,     0,
       0,    78,     0,     0,     0,    79,    66,    67,    68,    69,
      70,    71,    81,    72,    73,     0,     0,     0,    74,    75,
      76,    77,     0,     0,    78,     0,     0,     0,    79,     0,
     139,     0,     0,     0,     0,    81,    66,    67,    68,    69,
      70,    71,     0,    72,    73,     0,     0,     0,    74,    75,
      76,    77,     0,     0,    78,     0,     0,     0,    79,     0,
     145,     0,     0,     0,     0,    81,    66,    67,    68,    69,
      70,    71,     0,    72,    73,     0,     0,     0,    74,    75,
      76,    77,     0,   146,    78,     0,     0,     0,    79,    66,
      67,    68,    69,    70,    71,    81,    72,    73,     0,     0,
       0,    74,    75,    76,    77,     0,   147,    78,     0,     0,
       0,    79,    66,    67,    68,    69,    70,    71,    81,    72,
      73,     0,     0,     0,    74,    75,    76,    77,     0,     0,
      78,     0,     0,     0,    79,    66,    67,    68,    69,    70,
      71,    81,    72,     0,     0,     0,     0,    74,    75,    76,
      77,     0,     0,    78,     0,     0,     0,    79,    66,    67,
      68,    69,     0,     0,     0,     0,     0,     0,     0,     0,
      74,    75,    76,    77,     0,     0,    78,     0,     0,     0,
      79
};

static const short yycheck[] =
{
       4,     5,     6,     4,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    43,    44,
      24,    48,    47,    27,    28,    29,    51,    54,    30,    50,
      34,    41,    42,    43,    44,    47,     6,    47,    45,    51,
      45,    51,    45,    52,     0,     6,     4,    47,   123,    50,
      54,    55,    50,    57,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    -1,    81,    80,    83,
      -1,    -1,    -1,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    -1,   112,   113,
     114,   115,     4,    41,    42,    43,    44,   121,   122,    47,
      -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,   132,    -1,
      -1,    -1,   136,   137,   138,   139,    -1,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    54,    -1,     3,    -1,    58,     6,     7,     8,
       9,    10,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,    38,
      -1,    40,    -1,    42,    -1,    -1,    45,    -1,    47,    -1,
      -1,     3,    -1,    52,     6,     7,     8,     9,    10,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    -1,    -1,    38,    -1,    40,    -1,
      42,    -1,    -1,    45,    -1,    47,    -1,    -1,    -1,    -1,
      52,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    44,    -1,    46,    47,
      -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    -1,    57,
      58,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    -1,    -1,    41,    42,    43,    44,    -1,    46,    47,
      48,    -1,    -1,    51,    29,    30,    31,    32,    33,    34,
      58,    36,    37,    -1,    -1,    -1,    41,    42,    43,    44,
      -1,    46,    47,    48,    -1,    -1,    51,    29,    30,    31,
      32,    33,    34,    58,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    56,    -1,    58,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    57,    58,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    54,    -1,    -1,    -1,    58,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    54,    -1,    -1,    -1,    58,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,
      42,    43,    44,    -1,    46,    47,    -1,    -1,    -1,    51,
      29,    30,    31,    32,    33,    34,    58,    36,    37,    -1,
      -1,    -1,    41,    42,    43,    44,    -1,    -1,    47,    48,
      -1,    -1,    51,    29,    30,    31,    32,    33,    34,    58,
      36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    57,    58,    29,    30,    31,    32,    33,    34,    -1,
      36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,    55,
      -1,    -1,    58,    29,    30,    31,    32,    33,    34,    -1,
      36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,    -1,
      -1,    -1,    58,    29,    30,    31,    32,    33,    34,    -1,
      36,    37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,    -1,
      -1,    -1,    58,    29,    30,    31,    32,    33,    34,    -1,
      36,    37,    -1,    39,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    -1,    -1,    -1,    51,    29,    30,    31,    32,
      33,    34,    58,    36,    37,    -1,    -1,    -1,    41,    42,
      43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,
      53,    -1,    -1,    -1,    -1,    58,    29,    30,    31,    32,
      33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,
      43,    44,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,
      53,    -1,    -1,    -1,    -1,    58,    29,    30,    31,    32,
      33,    34,    -1,    36,    37,    -1,    -1,    -1,    41,    42,
      43,    44,    -1,    46,    47,    -1,    -1,    -1,    51,    29,
      30,    31,    32,    33,    34,    58,    36,    37,    -1,    -1,
      -1,    41,    42,    43,    44,    -1,    46,    47,    -1,    -1,
      -1,    51,    29,    30,    31,    32,    33,    34,    58,    36,
      37,    -1,    -1,    -1,    41,    42,    43,    44,    -1,    -1,
      47,    -1,    -1,    -1,    51,    29,    30,    31,    32,    33,
      34,    58,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
      44,    -1,    -1,    47,    -1,    -1,    -1,    51,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,
      51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     6,     7,     8,     9,    10,    11,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    35,    38,    40,    42,    45,    47,
      52,    60,    61,    62,    50,    62,    62,    62,     6,    63,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    45,    45,    62,    45,    52,    62,
      62,    62,    64,     6,    61,     0,    29,    30,    31,    32,
      33,    34,    36,    37,    41,    42,    43,    44,    47,    51,
      56,    58,    62,    50,    62,    62,    62,     6,    46,    57,
      57,    48,    54,     4,    53,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    61,
      62,    62,    54,    54,    46,     4,    62,    62,    62,    62,
      48,    57,     4,    54,    62,    62,    62,    62,    46,    48,
      46,    48,    55,    62,    62,    63,    54,    54,    39,    53,
      62,    62,    62,    62,    62,    53,    46,    46
};

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
#define YYABORT		goto yyabortlab
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
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

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
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
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



#if YYERROR_VERBOSE

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

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
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

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
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
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

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

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


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

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 2:
#line 54 "gram.y"
    { root = yyvsp[0].node; }
    break;

  case 3:
#line 58 "gram.y"
    { yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_EXPRLIST;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 4:
#line 65 "gram.y"
    { yyval.node = yyvsp[-1].node; }
    break;

  case 5:
#line 68 "gram.y"
    { yyval.node = yyvsp[0].node; }
    break;

  case 6:
#line 72 "gram.y"
    { yyval.node = yyvsp[-1].node; }
    break;

  case 7:
#line 75 "gram.y"
    { yyval.node = yyvsp[-1].node; }
    break;

  case 8:
#line 78 "gram.y"
    { yyval.node = yyvsp[-1].node; }
    break;

  case 9:
#line 81 "gram.y"
    { yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_GEN;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->ident = yyvsp[-5].ident;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;

  case 10:
#line 89 "gram.y"
    { yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = 0;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;

  case 11:
#line 97 "gram.y"
    { yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;

  case 12:
#line 105 "gram.y"
    { yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;

  case 13:
#line 113 "gram.y"
    { yyval.node = new_vector_node(2);
        yyval.node->type = NODETYPE_RANGE;
        yyval.node->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;

  case 14:
#line 121 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_ADD;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 15:
#line 129 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 16:
#line 137 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_SUB;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = new_scalar_node(0);
        yyval.node->expr[0]->type = NODETYPE_REAL;
        yyval.node->expr[0]->real = 0.0;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 17:
#line 147 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MUL;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 18:
#line 155 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_DIV;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 19:
#line 163 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_POW;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 20:
#line 171 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 21:
#line 179 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 22:
#line 187 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 23:
#line 195 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_GE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 24:
#line 203 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_EQ;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 25:
#line 211 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_NE;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 26:
#line 219 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_AND;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 27:
#line 227 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_OR;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 28:
#line 235 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_INDEX;
        yyval.node->pos = yyvsp[0].pos;
        yyval.node->expr[0] = yyvsp[-3].node;
        yyval.node->expr[1] = yyvsp[-1].node; }
    break;

  case 29:
#line 242 "gram.y"
    { yyval.node = new_node(1, node_is_scalar(yyvsp[0].node));
        yyval.node->type = NODETYPE_ASSIGN;
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->ident = yyvsp[-2].ident;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 30:
#line 249 "gram.y"
    { yyval.node = yyvsp[0].node; }
    break;

  case 31:
#line 252 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_NOT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 32:
#line 259 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SUM;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 33:
#line 265 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_PROD;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 34:
#line 271 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_AVG;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 35:
#line 277 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LEN;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 36:
#line 283 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MAX;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 37:
#line 289 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_MIN;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 38:
#line 295 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ISNAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 39:
#line 302 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SQRT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 40:
#line 309 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ABS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 41:
#line 316 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_EXP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 42:
#line 323 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_LOG;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 43:
#line 330 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_SIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 44:
#line 337 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_COS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 45:
#line 344 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_TAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 46:
#line 351 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ASIN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 47:
#line 358 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ACOS;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 48:
#line 365 "gram.y"
    { yyval.node = new_scalar_node(1);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_ATAN;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 49:
#line 372 "gram.y"
    { yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_CLAMP;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; }
    break;

  case 50:
#line 381 "gram.y"
    { yyval.node = new_scalar_node(3);
        yyval.node->pos = yyvsp[-7].pos;
        yyval.node->type = NODETYPE_SEGMENT;
        yyval.node->flags |= ALLARGS_SCALAR;
        yyval.node->expr[0] = yyvsp[-5].node;
        yyval.node->expr[1] = yyvsp[-3].node;
        yyval.node->expr[2] = yyvsp[-1].node; }
    break;

  case 51:
#line 390 "gram.y"
    { yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; }
    break;

  case 52:
#line 398 "gram.y"
    { yyval.node = new_node(3, node_is_scalar(yyvsp[-2].node));
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-4].node;
        yyval.node->expr[1] = yyvsp[-2].node;
        yyval.node->expr[2] = yyvsp[0].node; }
    break;

  case 53:
#line 406 "gram.y"
    { yyval.node = new_node(2, node_is_scalar(yyvsp[0].node));
        yyval.node->pos = yyvsp[-4].pos;
        yyval.node->type = NODETYPE_IFELSE;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 54:
#line 413 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-6].pos;
        yyval.node->type = NODETYPE_FOR;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 55:
#line 421 "gram.y"
    { yyval.node = new_node(0, ident_is_scalar(yyvsp[0].ident));
        yyval.node->type = NODETYPE_IDENT;
        yyval.node->pos = -1;
        yyval.node->ident = yyvsp[0].ident; }
    break;

  case 56:
#line 427 "gram.y"
    { yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = yyvsp[0].real; }
    break;

  case 57:
#line 433 "gram.y"
    { yyval.node = new_scalar_node(0);
        yyval.node->pos = -1;
        yyval.node->type = NODETYPE_REAL;
        yyval.node->real = INVALID_VALUE; }
    break;

  case 58:
#line 442 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->type = NODETYPE_LET;
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 59:
#line 449 "gram.y"
    { yyval.node = new_scalar_node(2);
        yyval.node->pos = yyvsp[-3].pos;
        yyval.node->type = NODETYPE_LET;
        yyval.node->ident = yyvsp[-4].ident;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;

  case 60:
#line 458 "gram.y"
    { yyval.node = new_vector_node(1);
        yyval.node->pos = yyvsp[0].node->pos;
        yyval.node->type = NODETYPE_VEC1;
        yyval.node->expr[0] = yyvsp[0].node; }
    break;

  case 61:
#line 463 "gram.y"
    { yyval.node = new_vector_node(2);
        yyval.node->pos = yyvsp[-1].pos;
        yyval.node->type = NODETYPE_VEC2;
        yyval.node->expr[0] = yyvsp[-2].node;
        yyval.node->expr[1] = yyvsp[0].node; }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1795 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


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


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
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
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


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
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 470 "gram.y"


node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}

