
#ifndef  DEF_BASIC
#define  DEF_BASIC

/* ----------------------------- MNI Header -----------------------------------
@NAME       : def_basic.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A set of macros and definitions useful for all MNI programs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 15, 1991       David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

#include  <stdlib.h>
#include  <stdio.h>
#include  <def_math.h>

/* --------- define the prefixes to all functions ---------- */

#define  public                /* functions called from outside file */
#define  private   static      /* functions/variables used only within file */

/* --------- define  TRUE and FALSE ------------------------ */

#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef  TRUE
#define  TRUE   1
#endif

#define  OFF     FALSE
#define  ON      TRUE

/* --------- macro to determine the size of a static array,
             e.g.,   int  array[] = { 1, 3, 9, 5 };           ------------ */

#define  SIZEOF_STATIC_ARRAY( array ) \
         (int) ( sizeof(array) / sizeof((array)[0]))

/* --------- interpolate between a and b ------------------- */

#define  INTERPOLATE( alpha, a, b ) ((a) + (alpha) * ((b) - (a)))

/* --------- PI, and angles -------------------------------- */

#define  PI           M_PI                  /* from math.h */

#define  DEG_TO_RAD   (PI / 180.0)
#define  RAD_TO_DEG   (180.0 / PI)

/* --------- Absolute value, min, and max.  Bear in mind that these
             may evaluate an expression multiple times, i.e., ABS( x - y ),
             and therefore may be inefficient, or incorrect,
             i.e, ABS( ++x );                          ------------------ */

#define  ABS( x )  ( ((x) > (0)) ? (x) : (-(x)) )

#undef   MAX
#define  MAX( x, y )  ( ((x) >= (y)) ? (x) : (y) )

#define  MAX3( x, y, z )  MAX( x, MAX(y,z) )

#undef   MIN
#define  MIN( x, y )  ( ((x) <= (y)) ? (x) : (y) )

#define  MIN3( x, y, z )  MIN( x, MIN(y,z) )

/* --------- gets the address of a 2-d array element in a 1-d array ----- */

#define  IJ( i, j, nj )          ( (i) * (nj) + (j) )

/* --------- gets the address of a 3-d array element in a 1-d array ----- */

#define  IJK( i, j, k, nj, nk )  ( (k) + (nk) * ((j) + (nj) * (i)) )

/* --------- environment variables -------------------------- */

#define  ENV_EXISTS( env ) ( getenv(env) != (char *) 0 )

/* --------- C and LINT stuff -------------------------- */

#ifdef sgi
#ifndef lint
#define USE_ANSI
#endif
#endif

#ifdef USE_ANSI
#define GLUE(x,y) x##y
#define CREATE_STRING(x) #x
#else
#define GLUE(x,y) x/**/y
#define CREATE_STRING(x) "x"
#endif

/* Basic types */

typedef  char            Smallest_int;

typedef  unsigned char   unsigned_byte;

typedef  int             Boolean;

typedef  double          Real;

#define  MAX_STRING_LENGTH    200

typedef  char     String[MAX_STRING_LENGTH+1];

/* --------------- */

#define  ROUND( x )     ((int) ((x) + 0.5))

#define  ROUND_UP( x )     ( (x) < 0.0 ? (int) ((x) - 0.5)      \
                                    : (int) ((x) + 0.5) )

#define  CEILING( x )   (  ((double)(int)(x) == (double)(x)) ?              \
                               ((int) (x)) :                                \
                               ((x) < 0.0 ? (int)(x) : ((int) (x)+1))       \
                        )

#define  FRACTION( x )  ((x) - (int) (x))

#define  IS_INT( x )    ((double) (x) == (double) ((int) x))

/* for loops */

#define  for_less( i, start, end )  for( i = start;  i < end;  ++i )

#define  for_inclusive( i, start, end )  for( i = start;  i <= end;  ++i )

#define  for_enum( e, max, type )  for( e = (type) 0;  e < max;  e = (type) ((int) e + 1) )

/* -------------------- Status --------------------- */

typedef enum { 
               OK,
               ERROR,
               WARNING,
               OUT_OF_MEMORY,
               INTERNAL_ERROR,
               QUIT
             } Status;

#define  HANDLE_INTERNAL_ERROR( X )                                           \
         {                                                                    \
             print( "Internal error:  %s\n", X );                            \
             abort_if_allowed();                                              \
         }

#endif
