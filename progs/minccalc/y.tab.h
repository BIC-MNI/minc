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

#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

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




#ifndef YYSTYPE
#line 15 "gram.y"
typedef union {
int      pos;
node_t   node;
double   real;
ident_t  ident;
} yystype;
/* Line 1281 of /usr/share/bison/yacc.c.  */
#line 127 "y.tab.h"
# define YYSTYPE yystype
#endif

extern YYSTYPE yylval;


#endif /* not BISON_Y_TAB_H */

