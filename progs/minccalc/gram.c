
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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


/* Line 189 of yacc.c  */
#line 88 "progs/minccalc/gram.c"

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
     IMAX = 271,
     IMIN = 272,
     ISNAN = 273,
     SQRT = 274,
     ABS = 275,
     EXP = 276,
     LOG = 277,
     SIN = 278,
     COS = 279,
     TAN = 280,
     ASIN = 281,
     ACOS = 282,
     ATAN = 283,
     CLAMP = 284,
     SEGMENT = 285,
     LT = 286,
     LE = 287,
     GT = 288,
     GE = 289,
     EQ = 290,
     NE = 291,
     NOT = 292,
     AND = 293,
     OR = 294,
     IF = 295,
     ELSE = 296,
     FOR = 297
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
#define IMAX 271
#define IMIN 272
#define ISNAN 273
#define SQRT 274
#define ABS 275
#define EXP 276
#define LOG 277
#define SIN 278
#define COS 279
#define TAN 280
#define ASIN 281
#define ACOS 282
#define ATAN 283
#define CLAMP 284
#define SEGMENT 285
#define LT 286
#define LE 287
#define GT 288
#define GE 289
#define EQ 290
#define NE 291
#define NOT 292
#define AND 293
#define OR 294
#define IF 295
#define ELSE 296
#define FOR 297




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 15 "gram.y"

int      pos;
node_t   node;
double   real;
ident_t  ident;



/* Line 214 of yacc.c  */
#line 217 "progs/minccalc/gram.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 229 "progs/minccalc/gram.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  69
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   800

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  6
/* YYNRULES -- Number of rules.  */
#define YYNRULES  63
/* YYNRULES -- Number of states.  */
#define YYNSTATES  152

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   297

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      47,    48,    45,    43,    56,    44,    51,    46,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    58,
       2,    52,     2,    60,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,    53,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    54,    57,    55,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42
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
     182,   185,   194,   203,   209,   217,   223,   231,   233,   235,
     237,   243,   249,   251
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      62,     0,    -1,    63,    -1,    64,    58,    63,    -1,    64,
      58,    -1,    64,    -1,    47,    64,    48,    -1,    54,    63,
      55,    -1,    49,    66,    50,    -1,    54,     6,     4,    64,
      57,    64,    55,    -1,    47,    64,    59,    64,    48,    -1,
      47,    64,    59,    64,    50,    -1,    49,    64,    59,    64,
      48,    -1,    49,    64,    59,    64,    50,    -1,    64,    43,
      64,    -1,    64,    44,    64,    -1,    44,    64,    -1,    64,
      45,    64,    -1,    64,    46,    64,    -1,    64,    53,    64,
      -1,    64,    31,    64,    -1,    64,    32,    64,    -1,    64,
      33,    64,    -1,    64,    34,    64,    -1,    64,    35,    64,
      -1,    64,    36,    64,    -1,    64,    38,    64,    -1,    64,
      39,    64,    -1,    64,    49,    64,    50,    -1,     6,    52,
      64,    -1,    11,    65,    -1,    37,    64,    -1,    10,    64,
      -1,     9,    64,    -1,     8,    64,    -1,    13,    64,    -1,
      14,    64,    -1,    15,    64,    -1,    16,    64,    -1,    17,
      64,    -1,    18,    64,    -1,    19,    64,    -1,    20,    64,
      -1,    21,    64,    -1,    22,    64,    -1,    23,    64,    -1,
      24,    64,    -1,    25,    64,    -1,    26,    64,    -1,    27,
      64,    -1,    28,    64,    -1,    29,    47,    64,    56,    64,
      56,    64,    48,    -1,    30,    47,    64,    56,    64,    56,
      64,    48,    -1,    64,    60,    64,    59,    64,    -1,    40,
      47,    64,    48,    64,    41,    64,    -1,    40,    47,    64,
      48,    64,    -1,    42,    54,     6,     4,    64,    55,    64,
      -1,     6,    -1,     7,    -1,     3,    -1,     6,    52,    64,
      56,    65,    -1,     6,    52,    64,     4,    64,    -1,    64,
      -1,    66,    56,    64,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    53,    53,    57,    64,    67,    71,    74,    77,    80,
      88,    96,   104,   112,   120,   128,   136,   146,   154,   162,
     170,   178,   186,   194,   202,   210,   218,   226,   234,   241,
     248,   251,   258,   264,   270,   276,   282,   288,   294,   300,
     306,   313,   320,   327,   334,   341,   348,   355,   362,   369,
     376,   383,   392,   401,   409,   417,   424,   432,   438,   444,
     453,   460,   469,   474
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAN", "IN", "TO", "IDENT", "REAL",
  "AVG", "PROD", "SUM", "LET", "NEG", "LEN", "MAX", "MIN", "IMAX", "IMIN",
  "ISNAN", "SQRT", "ABS", "EXP", "LOG", "SIN", "COS", "TAN", "ASIN",
  "ACOS", "ATAN", "CLAMP", "SEGMENT", "LT", "LE", "GT", "GE", "EQ", "NE",
  "NOT", "AND", "OR", "IF", "ELSE", "FOR", "'+'", "'-'", "'*'", "'/'",
  "'('", "')'", "'['", "']'", "'.'", "'='", "'^'", "'{'", "'}'", "','",
  "'|'", "';'", "':'", "'?'", "$accept", "top", "exprlist", "expr",
  "letexpr", "vector", 0
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
     295,   296,   297,    43,    45,    42,    47,    40,    41,    91,
      93,    46,    61,    94,   123,   125,    44,   124,    59,    58,
      63
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    63,    63,    63,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      65,    65,    66,    66
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     1,     3,     3,     3,     7,
       5,     5,     5,     5,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     4,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     8,     8,     5,     7,     5,     7,     1,     1,     1,
       5,     5,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    59,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     2,     5,     0,    34,    33,    32,
       0,    30,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,     0,     0,
      31,     0,     0,    16,     0,    62,     0,    57,     0,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     4,     0,    29,     0,     0,     0,
       0,     0,     6,     0,     0,     8,     0,     0,     7,    20,
      21,    22,    23,    24,    25,    26,    27,    14,    15,    17,
      18,     0,    19,     3,     0,     0,     0,     0,     0,     0,
       0,     0,    63,     0,    28,     0,     0,     0,     0,     0,
      55,     0,    10,    11,    12,    13,     0,    53,    61,    60,
       0,     0,     0,     0,     0,     0,     0,    54,    56,     9,
      51,    52
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    33,    34,    35,    41,    66
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -48
static const yytype_int16 yypact[] =
{
     167,   -48,   -21,   -48,   167,   167,   167,    27,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,     6,    11,   167,    12,   -11,   167,
     167,   167,   216,    45,   -48,     3,   167,     1,     1,     1,
       8,   -48,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,   167,   167,
      -9,   167,    56,    -9,   240,   316,   -27,    -1,     9,   -48,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   701,   167,   346,   376,
     406,    61,   -48,   167,   167,   -48,   167,   167,   -48,    42,
      42,    42,    42,   747,   747,    74,   724,   -25,   -25,    -9,
      -9,   429,    -9,   -48,   452,   112,   167,   167,   167,   167,
     270,   293,   701,   482,   -48,   167,   167,    27,   512,   542,
     572,   595,   -48,   -48,   -48,   -48,   167,   701,   701,   -48,
     167,   167,   167,   167,   625,   655,   678,   701,   701,   -48,
     -48,   -48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -48,   -48,    -2,    -4,   -47,   -48
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      37,    38,    39,    97,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      80,    81,    60,    95,    82,    63,    64,    65,    83,    96,
      68,    36,    86,    40,    70,    71,    72,    73,    74,    75,
      82,    76,    77,    62,    83,    69,    78,    79,    80,    81,
      82,    36,    82,    58,    88,    89,    83,    90,    59,    61,
      87,    84,    91,    85,    98,   119,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     139,   114,   113,   115,     0,    78,    79,    80,    81,   120,
     121,    82,   122,   123,     0,    83,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    70,    71,    72,    73,    74,
      75,     0,   128,   129,   130,   131,   126,    78,    79,    80,
      81,   137,   138,    82,     0,     0,     0,    83,     0,     0,
       0,     0,   144,     0,     0,     0,   145,   146,   147,   148,
       0,     0,     0,    70,    71,    72,    73,    74,    75,     0,
      76,    77,     0,     0,     0,    78,    79,    80,    81,     0,
       0,    82,     0,     0,     0,    83,     0,     0,   127,     0,
       1,     0,    85,     2,     3,     4,     5,     6,     7,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,     0,
       0,     0,     0,     0,    26,     0,     0,    27,     0,    28,
       0,    29,     0,     0,    30,     0,    31,     0,     0,     1,
       0,    32,    67,     3,     4,     5,     6,     7,     0,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,     0,     0,     0,
       0,     0,     0,    26,     0,     0,    27,     0,    28,     0,
      29,     0,     0,    30,     0,    31,     0,     0,     0,     0,
      32,    70,    71,    72,    73,    74,    75,     0,    76,    77,
       0,     0,     0,    78,    79,    80,    81,     0,    92,    82,
       0,     0,     0,    83,     0,     0,     0,     0,     0,    93,
      85,    70,    71,    72,    73,    74,    75,     0,    76,    77,
       0,     0,     0,    78,    79,    80,    81,     0,   132,    82,
     133,     0,     0,    83,    70,    71,    72,    73,    74,    75,
      85,    76,    77,     0,     0,     0,    78,    79,    80,    81,
       0,   134,    82,   135,     0,     0,    83,    70,    71,    72,
      73,    74,    75,    85,    76,    77,     0,     0,     0,    78,
      79,    80,    81,     0,     0,    82,     0,     0,     0,    83,
       0,     0,     0,     0,     0,    94,    85,    70,    71,    72,
      73,    74,    75,     0,    76,    77,     0,     0,     0,    78,
      79,    80,    81,     0,     0,    82,     0,     0,     0,    83,
       0,     0,   116,     0,     0,     0,    85,    70,    71,    72,
      73,    74,    75,     0,    76,    77,     0,     0,     0,    78,
      79,    80,    81,     0,     0,    82,     0,     0,     0,    83,
       0,     0,   117,     0,     0,     0,    85,    70,    71,    72,
      73,    74,    75,     0,    76,    77,     0,     0,     0,    78,
      79,    80,    81,     0,   118,    82,     0,     0,     0,    83,
      70,    71,    72,    73,    74,    75,    85,    76,    77,     0,
       0,     0,    78,    79,    80,    81,     0,     0,    82,   124,
       0,     0,    83,    70,    71,    72,    73,    74,    75,    85,
      76,    77,     0,     0,     0,    78,    79,    80,    81,     0,
       0,    82,     0,     0,     0,    83,     0,     0,     0,     0,
       0,   125,    85,    70,    71,    72,    73,    74,    75,     0,
      76,    77,     0,     0,     0,    78,    79,    80,    81,     0,
       0,    82,     0,     0,     0,    83,     0,     0,     0,   136,
       0,     0,    85,    70,    71,    72,    73,    74,    75,     0,
      76,    77,     0,     0,     0,    78,    79,    80,    81,     0,
       0,    82,     0,     0,     0,    83,     0,     0,   140,     0,
       0,     0,    85,    70,    71,    72,    73,    74,    75,     0,
      76,    77,     0,     0,     0,    78,    79,    80,    81,     0,
       0,    82,     0,     0,     0,    83,     0,     0,   141,     0,
       0,     0,    85,    70,    71,    72,    73,    74,    75,     0,
      76,    77,     0,   142,     0,    78,    79,    80,    81,     0,
       0,    82,     0,     0,     0,    83,    70,    71,    72,    73,
      74,    75,    85,    76,    77,     0,     0,     0,    78,    79,
      80,    81,     0,     0,    82,     0,     0,     0,    83,     0,
     143,     0,     0,     0,     0,    85,    70,    71,    72,    73,
      74,    75,     0,    76,    77,     0,     0,     0,    78,    79,
      80,    81,     0,     0,    82,     0,     0,     0,    83,     0,
     149,     0,     0,     0,     0,    85,    70,    71,    72,    73,
      74,    75,     0,    76,    77,     0,     0,     0,    78,    79,
      80,    81,     0,   150,    82,     0,     0,     0,    83,    70,
      71,    72,    73,    74,    75,    85,    76,    77,     0,     0,
       0,    78,    79,    80,    81,     0,   151,    82,     0,     0,
       0,    83,    70,    71,    72,    73,    74,    75,    85,    76,
      77,     0,     0,     0,    78,    79,    80,    81,     0,     0,
      82,     0,     0,     0,    83,    70,    71,    72,    73,    74,
      75,    85,    76,     0,     0,     0,     0,    78,    79,    80,
      81,     0,     0,    82,     0,     0,     0,    83,    70,    71,
      72,    73,     0,     0,     0,     0,     0,     0,     0,     0,
      78,    79,    80,    81,     0,     0,    82,     0,     0,     0,
      83
};

static const yytype_int16 yycheck[] =
{
       4,     5,     6,     4,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      45,    46,    26,    50,    49,    29,    30,    31,    53,    56,
      32,    52,    36,     6,    31,    32,    33,    34,    35,    36,
      49,    38,    39,    54,    53,     0,    43,    44,    45,    46,
      49,    52,    49,    47,    58,    59,    53,    61,    47,    47,
      52,    58,     6,    60,    55,     4,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
     127,    85,    84,    87,    -1,    43,    44,    45,    46,    93,
      94,    49,    96,    97,    -1,    53,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,
      36,    -1,   116,   117,   118,   119,     4,    43,    44,    45,
      46,   125,   126,    49,    -1,    -1,    -1,    53,    -1,    -1,
      -1,    -1,   136,    -1,    -1,    -1,   140,   141,   142,   143,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    -1,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    56,    -1,
       3,    -1,    60,     6,     7,     8,     9,    10,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    42,
      -1,    44,    -1,    -1,    47,    -1,    49,    -1,    -1,     3,
      -1,    54,     6,     7,     8,     9,    10,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    42,    -1,
      44,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,
      54,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    -1,    -1,    43,    44,    45,    46,    -1,    48,    49,
      -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,    -1,    59,
      60,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    -1,    -1,    43,    44,    45,    46,    -1,    48,    49,
      50,    -1,    -1,    53,    31,    32,    33,    34,    35,    36,
      60,    38,    39,    -1,    -1,    -1,    43,    44,    45,    46,
      -1,    48,    49,    50,    -1,    -1,    53,    31,    32,    33,
      34,    35,    36,    60,    38,    39,    -1,    -1,    -1,    43,
      44,    45,    46,    -1,    -1,    49,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    -1,    -1,    59,    60,    31,    32,    33,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    43,
      44,    45,    46,    -1,    -1,    49,    -1,    -1,    -1,    53,
      -1,    -1,    56,    -1,    -1,    -1,    60,    31,    32,    33,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    43,
      44,    45,    46,    -1,    -1,    49,    -1,    -1,    -1,    53,
      -1,    -1,    56,    -1,    -1,    -1,    60,    31,    32,    33,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    43,
      44,    45,    46,    -1,    48,    49,    -1,    -1,    -1,    53,
      31,    32,    33,    34,    35,    36,    60,    38,    39,    -1,
      -1,    -1,    43,    44,    45,    46,    -1,    -1,    49,    50,
      -1,    -1,    53,    31,    32,    33,    34,    35,    36,    60,
      38,    39,    -1,    -1,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
      -1,    59,    60,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    -1,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    -1,    57,
      -1,    -1,    60,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    -1,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    56,    -1,
      -1,    -1,    60,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    -1,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    56,    -1,
      -1,    -1,    60,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    41,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    -1,    -1,    -1,    53,    31,    32,    33,    34,
      35,    36,    60,    38,    39,    -1,    -1,    -1,    43,    44,
      45,    46,    -1,    -1,    49,    -1,    -1,    -1,    53,    -1,
      55,    -1,    -1,    -1,    -1,    60,    31,    32,    33,    34,
      35,    36,    -1,    38,    39,    -1,    -1,    -1,    43,    44,
      45,    46,    -1,    -1,    49,    -1,    -1,    -1,    53,    -1,
      55,    -1,    -1,    -1,    -1,    60,    31,    32,    33,    34,
      35,    36,    -1,    38,    39,    -1,    -1,    -1,    43,    44,
      45,    46,    -1,    48,    49,    -1,    -1,    -1,    53,    31,
      32,    33,    34,    35,    36,    60,    38,    39,    -1,    -1,
      -1,    43,    44,    45,    46,    -1,    48,    49,    -1,    -1,
      -1,    53,    31,    32,    33,    34,    35,    36,    60,    38,
      39,    -1,    -1,    -1,    43,    44,    45,    46,    -1,    -1,
      49,    -1,    -1,    -1,    53,    31,    32,    33,    34,    35,
      36,    60,    38,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    -1,    -1,    49,    -1,    -1,    -1,    53,    31,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    -1,    -1,    49,    -1,    -1,    -1,
      53
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,     7,     8,     9,    10,    11,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    37,    40,    42,    44,
      47,    49,    54,    62,    63,    64,    52,    64,    64,    64,
       6,    65,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    47,    47,
      64,    47,    54,    64,    64,    64,    66,     6,    63,     0,
      31,    32,    33,    34,    35,    36,    38,    39,    43,    44,
      45,    46,    49,    53,    58,    60,    64,    52,    64,    64,
      64,     6,    48,    59,    59,    50,    56,     4,    55,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    63,    64,    64,    56,    56,    48,     4,
      64,    64,    64,    64,    50,    59,     4,    56,    64,    64,
      64,    64,    48,    50,    48,    50,    57,    64,    64,    65,
      56,    56,    41,    55,    64,    64,    64,    64,    64,    55,
      48,    48
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
#line 54 "gram.y"
    { root = (yyvsp[(1) - (1)].node); }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 58 "gram.y"
    { (yyval.node) = new_node(2, node_is_scalar((yyvsp[(3) - (3)].node)));
        (yyval.node)->type = NODETYPE_EXPRLIST;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 65 "gram.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 68 "gram.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 72 "gram.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 75 "gram.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 78 "gram.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 81 "gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_GEN;
        (yyval.node)->pos = (yyvsp[(7) - (7)].pos);
        (yyval.node)->ident = (yyvsp[(2) - (7)].ident);
        (yyval.node)->expr[0] = (yyvsp[(4) - (7)].node);
        (yyval.node)->expr[1] = (yyvsp[(6) - (7)].node); }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 89 "gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = 0;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 97 "gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = RANGE_EXACT_UPPER;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 105 "gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = RANGE_EXACT_LOWER;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 113 "gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->type = NODETYPE_RANGE;
        (yyval.node)->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        (yyval.node)->pos = (yyvsp[(5) - (5)].pos);
        (yyval.node)->expr[0] = (yyvsp[(2) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(4) - (5)].node); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 121 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_ADD;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 129 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_SUB;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 137 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_SUB;
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = new_scalar_node(0);
        (yyval.node)->expr[0]->type = NODETYPE_REAL;
        (yyval.node)->expr[0]->real = 0.0;
        (yyval.node)->expr[1] = (yyvsp[(2) - (2)].node); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 147 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_MUL;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 155 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_DIV;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 163 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_POW;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 171 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_LT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 179 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_LE;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 187 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_GT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 195 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_GE;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 203 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_EQ;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 211 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_NE;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 219 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_AND;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 227 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_OR;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 235 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_INDEX;
        (yyval.node)->pos = (yyvsp[(4) - (4)].pos);
        (yyval.node)->expr[0] = (yyvsp[(1) - (4)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (4)].node); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 242 "gram.y"
    { (yyval.node) = new_node(1, node_is_scalar((yyvsp[(3) - (3)].node)));
        (yyval.node)->type = NODETYPE_ASSIGN;
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->ident = (yyvsp[(1) - (3)].ident);
        (yyval.node)->expr[0] = (yyvsp[(3) - (3)].node); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 249 "gram.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 252 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_NOT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 259 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_SUM;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 265 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_PROD;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 271 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_AVG;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 277 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_LEN;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 283 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_MAX;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 289 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_MIN;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 295 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_IMAX;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 301 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_IMIN;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 307 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ISNAN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 314 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_SQRT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 321 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ABS;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 328 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_EXP;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 335 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_LOG;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 342 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_SIN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 349 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_COS;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 356 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_TAN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 363 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ASIN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 370 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ACOS;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 377 "gram.y"
    { (yyval.node) = new_scalar_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (2)].pos);
        (yyval.node)->type = NODETYPE_ATAN;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(2) - (2)].node); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 384 "gram.y"
    { (yyval.node) = new_scalar_node(3);
        (yyval.node)->pos = (yyvsp[(1) - (8)].pos);
        (yyval.node)->type = NODETYPE_CLAMP;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(3) - (8)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (8)].node);
        (yyval.node)->expr[2] = (yyvsp[(7) - (8)].node); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 393 "gram.y"
    { (yyval.node) = new_scalar_node(3);
        (yyval.node)->pos = (yyvsp[(1) - (8)].pos);
        (yyval.node)->type = NODETYPE_SEGMENT;
        (yyval.node)->flags |= ALLARGS_SCALAR;
        (yyval.node)->expr[0] = (yyvsp[(3) - (8)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (8)].node);
        (yyval.node)->expr[2] = (yyvsp[(7) - (8)].node); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 402 "gram.y"
    { (yyval.node) = new_node(3, node_is_scalar((yyvsp[(3) - (5)].node)));
        (yyval.node)->pos = (yyvsp[(2) - (5)].pos);
        (yyval.node)->type = NODETYPE_IFELSE;
        (yyval.node)->expr[0] = (yyvsp[(1) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[2] = (yyvsp[(5) - (5)].node); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 410 "gram.y"
    { (yyval.node) = new_node(3, node_is_scalar((yyvsp[(5) - (7)].node)));
        (yyval.node)->pos = (yyvsp[(1) - (7)].pos);
        (yyval.node)->type = NODETYPE_IFELSE;
        (yyval.node)->expr[0] = (yyvsp[(3) - (7)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (7)].node);
        (yyval.node)->expr[2] = (yyvsp[(7) - (7)].node); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 418 "gram.y"
    { (yyval.node) = new_node(2, node_is_scalar((yyvsp[(5) - (5)].node)));
        (yyval.node)->pos = (yyvsp[(1) - (5)].pos);
        (yyval.node)->type = NODETYPE_IFELSE;
        (yyval.node)->expr[0] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (5)].node); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 425 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(1) - (7)].pos);
        (yyval.node)->type = NODETYPE_FOR;
        (yyval.node)->ident = (yyvsp[(3) - (7)].ident);
        (yyval.node)->expr[0] = (yyvsp[(5) - (7)].node);
        (yyval.node)->expr[1] = (yyvsp[(7) - (7)].node); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 433 "gram.y"
    { (yyval.node) = new_node(0, ident_is_scalar((yyvsp[(1) - (1)].ident)));
        (yyval.node)->type = NODETYPE_IDENT;
        (yyval.node)->pos = -1;
        (yyval.node)->ident = (yyvsp[(1) - (1)].ident); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 439 "gram.y"
    { (yyval.node) = new_scalar_node(0);
        (yyval.node)->pos = -1;
        (yyval.node)->type = NODETYPE_REAL;
        (yyval.node)->real = (yyvsp[(1) - (1)].real); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 445 "gram.y"
    { (yyval.node) = new_scalar_node(0);
        (yyval.node)->pos = -1;
        (yyval.node)->type = NODETYPE_REAL;
        (yyval.node)->real = INVALID_VALUE; }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 454 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->type = NODETYPE_LET;
        (yyval.node)->pos = (yyvsp[(2) - (5)].pos);
        (yyval.node)->ident = (yyvsp[(1) - (5)].ident);
        (yyval.node)->expr[0] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (5)].node); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 461 "gram.y"
    { (yyval.node) = new_scalar_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (5)].pos);
        (yyval.node)->type = NODETYPE_LET;
        (yyval.node)->ident = (yyvsp[(1) - (5)].ident);
        (yyval.node)->expr[0] = (yyvsp[(3) - (5)].node);
        (yyval.node)->expr[1] = (yyvsp[(5) - (5)].node); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 470 "gram.y"
    { (yyval.node) = new_vector_node(1);
        (yyval.node)->pos = (yyvsp[(1) - (1)].node)->pos;
        (yyval.node)->type = NODETYPE_VEC1;
        (yyval.node)->expr[0] = (yyvsp[(1) - (1)].node); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 475 "gram.y"
    { (yyval.node) = new_vector_node(2);
        (yyval.node)->pos = (yyvsp[(2) - (3)].pos);
        (yyval.node)->type = NODETYPE_VEC2;
        (yyval.node)->expr[0] = (yyvsp[(1) - (3)].node);
        (yyval.node)->expr[1] = (yyvsp[(3) - (3)].node); }
    break;



/* Line 1455 of yacc.c  */
#line 2348 "progs/minccalc/gram.c"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1675 of yacc.c  */
#line 482 "gram.y"


node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}

