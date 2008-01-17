/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

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
/* Tokens.  */
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
#line 1 "minccalc/gram.y"

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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 15 "minccalc/gram.y"
{
int      pos;
node_t   node;
double   real;
ident_t  ident;
}
/* Line 187 of yacc.c.  */
#line 197 "minccalc/gram.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 210 "minccalc/gram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  65
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   820

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  6
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNRULES -- Number of states.  */
#define YYNSTATES  148

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    14,    18,    22,    26,
      34,    40,    46,    52,    58,    62,    66,    69,    73,    77,
      81,    85,    89,    93,    97,   101,   105,   109,   113,   118,
     122,   125,   128,   131,   134,   137,   140,   143,   146,   149,
     152,   155,   158,   161,   164,   167,   170,   173,   176,   179,
     188,   197,   203,   211,   217,   225,   227,   229,   231,   237,
     243,   245
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
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
static const yytype_uint16 yyrline[] =
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

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAN", "IN", "TO", "IDENT", "REAL",
  "AVG", "PROD", "SUM", "LET", "NEG", "LEN", "MAX", "MIN", "ISNAN", "SQRT",
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
static const yytype_uint16 yytoknum[] =
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
static const yytype_uint8 yyr1[] =
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
static const yytype_uint8 yyr2[] =
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
static const yytype_uint8 yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    31,    32,    33,    39,    62
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -76
static const yytype_int16 yypact[] =
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
static const yytype_int8 yypgoto[] =
{
     -76,   -76,    -2,    -4,   -75,   -76
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
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

static const yytype_int16 yycheck[] =
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
static const yytype_uint8 yystos[] =
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

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 54 "minccalc/gram.y"
    { root = (yyvsp[(1) - (1)].node); ;}
    break;

  case 3:
#line 58 "minccalc/gram.y"
    { (yyval.node) = new_node(2, node_is_scalar((yyvsp[(3) - (3)].node)));
        (yyval.node)->type = NODETYPE_EXPRLIST;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 4:
#line 65 "minccalc/gram.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 5:
#line 68 "minccalc/gram.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 6:
#line 72 "minccalc/gram.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 7:
#line 75 "minccalc/gram.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 8:
#line 78 "minccalc/gram.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 9:
#line 81 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_GEN;
        (yyval.node)->pos = (yyvsp[(7) - (7)].pos);
        (yyval.node)->ident = (yyvsp[(2) - (7)].ident);
        (yyval.node)->expr[0] = (yyvsp[(4) - (7)].node);
        (yyval.node)->expr[1] = (yyvsp[(6) - (7)].node); ;}
    break;

  case 10:
#line 89 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = 0;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); ;}
    break;

  case 11:
#line 97 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = RANGE_EXACT_UPPER;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); ;}
    break;

  case 12:
#line 105 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = RANGE_EXACT_LOWER;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); ;}
    break;

  case 13:
#line 113 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); ;}
    break;

  case 14:
#line 121 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_ADD;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 15:
#line 129 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_SUB;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 16:
#line 137 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_SUB;
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = new_scalar_node(0);
        (yyval.node)->expr[0]->type = NODETYPE_REAL;
        (yyval.node)->expr[0]->real = 0.0;
        (yyval.node)->expr[1] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 17:
#line 147 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_MUL;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 18:
#line 155 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_DIV;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 19:
#line 163 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_POW;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 20:
#line 171 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_LT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 21:
#line 179 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_LE;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 22:
#line 187 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_GT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 23:
#line 195 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_GE;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 24:
#line 203 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_EQ;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 25:
#line 211 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_NE;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 26:
#line 219 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_AND;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 27:
#line 227 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_OR;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 28:
#line 235 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_INDEX;
        (yyval.node)->pos = (yyvsp[(4) - (4)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (4)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (4)].node); ;}
    break;

  case 29:
#line 242 "minccalc/gram.y"
    { (yyval.node) = new_node(1, node_is_scalar((yyvsp[(3) - (3)].node)));
        (yyval.node)->type = NODETYPE_ASSIGN;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->ident = (yyvsp[(1) - (3)].ident);
        (yyval.node)->expr[0] = (yyvsp[(3) - (3)].node); ;}
    break;

  case 30:
#line 249 "minccalc/gram.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); ;}
    break;

  case 31:
#line 252 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_NOT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 32:
#line 259 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_SUM;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 33:
#line 265 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_PROD;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 34:
#line 271 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_AVG;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 35:
#line 277 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_LEN;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 36:
#line 283 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_MAX;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 37:
#line 289 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_MIN;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 38:
#line 295 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ISNAN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 39:
#line 302 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_SQRT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 40:
#line 309 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ABS;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 41:
#line 316 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_EXP;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 42:
#line 323 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_LOG;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 43:
#line 330 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_SIN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 44:
#line 337 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_COS;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 45:
#line 344 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_TAN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 46:
#line 351 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ASIN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 47:
#line 358 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ACOS;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 48:
#line 365 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ATAN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); ;}
    break;

  case 49:
#line 372 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(3);
        (yyval.node)->pos = (yyvsp[(1) - (8)].pos);
        (yyval.node)->type = NODETYPE_CLAMP;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(3) - (8)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (8)].node);
        (yyval.node)->expr[2] = (yyvsp[(7) - (8)].node); ;}
    break;

  case 50:
#line 381 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(3);
        (yyval.node)->pos = (yyvsp[(1) - (8)].pos);
        (yyval.node)->type = NODETYPE_SEGMENT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(3) - (8)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (8)].node);
        (yyval.node)->expr[2] = (yyvsp[(7) - (8)].node); ;}
    break;

  case 51:
#line 390 "minccalc/gram.y"
    { (yyval.node) = new_node(3, node_is_scalar((yyvsp[(3) - (5)].node)));
        (yyval.node)->pos = (yyvsp[(2) - (5)].pos);
        (yyval.node)->type = NODETYPE_IFELSE;
        (yyval.node)->expr[0] = (yyvsp[(1) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[2] = (yyvsp[(5) - (5)].node); ;}
    break;

  case 52:
#line 398 "minccalc/gram.y"
    { (yyval.node) = new_node(3, node_is_scalar((yyvsp[(5) - (7)].node)));
        (yyval.node)->pos = (yyvsp[(1) - (7)].pos);
        (yyval.node)->type = NODETYPE_IFELSE;
        (yyval.node)->expr[0] = (yyvsp[(3) - (7)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (7)].node);
        (yyval.node)->expr[2] = (yyvsp[(7) - (7)].node); ;}
    break;

  case 53:
#line 406 "minccalc/gram.y"
    { (yyval.node) = new_node(2, node_is_scalar((yyvsp[(5) - (5)].node)));
        (yyval.node)->pos = (yyvsp[(1) - (5)].pos);
        (yyval.node)->type = NODETYPE_IFELSE;
        (yyval.node)->expr[0] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (5)].node); ;}
    break;

  case 54:
#line 413 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(1) - (7)].pos);
        (yyval.node)->type = NODETYPE_FOR;
        (yyval.node)->ident = (yyvsp[(3) - (7)].ident);
        (yyval.node)->expr[0] = (yyvsp[(5) - (7)].node);
        (yyval.node)->expr[1] = (yyvsp[(7) - (7)].node); ;}
    break;

  case 55:
#line 421 "minccalc/gram.y"
    { (yyval.node) = new_node(0, ident_is_scalar((yyvsp[(1) - (1)].ident)));
        (yyval.node)->type = NODETYPE_IDENT;
        (yyval.node)->pos = -1;
        (yyval.node)->ident = (yyvsp[(1) - (1)].ident); ;}
    break;

  case 56:
#line 427 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(0);
        (yyval.node)->pos = -1;
        (yyval.node)->type = NODETYPE_REAL;
        (yyval.node)->real = (yyvsp[(1) - (1)].real); ;}
    break;

  case 57:
#line 433 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(0);
        (yyval.node)->pos = -1;
        (yyval.node)->type = NODETYPE_REAL;
        (yyval.node)->real = INVALID_VALUE; ;}
    break;

  case 58:
#line 442 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_LET;
        (yyval.node)->pos = (yyvsp[(2) - (5)].pos);
        (yyval.node)->ident = (yyvsp[(1) - (5)].ident);
        (yyval.node)->expr[0] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (5)].node); ;}
    break;

  case 59:
#line 449 "minccalc/gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (5)].pos);
        (yyval.node)->type = NODETYPE_LET;
        (yyval.node)->ident = (yyvsp[(1) - (5)].ident);
        (yyval.node)->expr[0] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (5)].node); ;}
    break;

  case 60:
#line 458 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (1)].node)->pos;
        (yyval.node)->type = NODETYPE_VEC1;
        (yyval.node)->expr[0] = (yyvsp[(1) - (1)].node); ;}
    break;

  case 61:
#line 463 "minccalc/gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_VEC2;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2189 "minccalc/gram.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 470 "minccalc/gram.y"


node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}

