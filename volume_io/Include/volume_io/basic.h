
#ifndef  DEF_BASIC
#define  DEF_BASIC

#ifndef lint
static char basic_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/basic.h,v 1.19 1995-03-21 19:01:43 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : basic.h
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
#include  <memory.h>     /* --- for memcpy, etc. */
#include  <def_math.h>

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

#define  ABS( x )   ( ((x) > 0) ? (x) : (-(x)) )
#define  SIGN( x )  ( ((x) > 0) ? 1 : (((x) < 0) ? -1 : 0) )

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

#ifndef lint
#define USE_ANSI
#endif

#ifdef USE_ANSI
#define GLUE(x,y) x##y
#define GLUE3(x,y,z) x##y##z
#define CREATE_STRING(x) #x
#else
#define GLUE(x,y) x/**/y
#define GLUE3(x,y,z) x/**/y/**/z
#define CREATE_STRING(x) "x"
#endif

/* Basic types */

typedef  char            Smallest_int;

typedef  unsigned char   unsigned_byte;

typedef  int             BOOLEAN;

typedef  double          Real;

#define  MAX_STRING_LENGTH    200

typedef  char     STRING[MAX_STRING_LENGTH+1];

/* --------------- */

#define  IS_INT( x )    ((double) (x) == (double) ((int) (x)))

#define  FLOOR( x )   (((x) < 0.0) ? ((IS_INT(x) ? (int) (x) : (int) (x) - 1)) \
                                   : (int) (x) )

#define  ROUND( x )     FLOOR( (x) + 0.5 )

#define  POSITIVE_ROUND( x )     ((int)( (x) + 0.5 ))

#define  CEILING( x )   (  IS_INT(x) ?                                      \
                               ((int) (x)) :                                \
                               ((x) < 0.0 ? (int)(x) : ((int) (x)+1))       \
                        )

#define  FRACTION( x )  ((x) - FLOOR(x))

/* for loops */

#define  for_less( i, start, end )  for( (i) = (start);  (i) < (end);  ++(i) )

#define  for_down( i, start, end )  for( (i) = (start);  (i) >= (end); --(i))

#define  for_inclusive( i, start, end )  \
                   for( (i) = (start);  (i) <= (end);  ++(i) )

#define  for_enum( e, max, type )  \
                for( (e) = (type) 0;  (e) < (max);  (e) = (type) ((int) (e)+1) )

#define  CONVERT_INTEGER_RANGE( x1, min1, max1, min2, max2 )                  \
              ((min2) + (2 * (x1) + 1 - 2 * (min1)) * ((max2) - (min2) + 1) / \
                                                      ((max1) - (min1) + 1) / 2)

/* -------------------- Status --------------------- */

typedef enum { 
               OK,
               ERROR,
               INTERNAL_ERROR,
               END_OF_FILE,
               QUIT
             } Status;

#define  HANDLE_INTERNAL_ERROR( X )                                           \
         handle_internal_error( X )

#endif
