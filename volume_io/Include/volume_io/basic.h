#ifndef  DEF_BASIC
#define  DEF_BASIC

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
@VERSION    : $Header: /private-cvsroot/minc/volume_io/Include/volume_io/basic.h,v 1.32 2001-12-14 17:12:24 neelin Exp $
---------------------------------------------------------------------------- */

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

#ifdef __sgi
#include  <string.h>     /* --- for memcpy, etc. */
#else
#include  <memory.h>     /* --- for memcpy, etc. */
#endif

#include  <volume_io/def_math.h>
#include  <volume_io/system_dependent.h>

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
#define  FABS( x )   fabs( (double) x )
#define  SIGN( x )  ( ((x) > 0) ? 1 : (((x) < 0) ? -1 : 0) )
#define  FSIGN( x )  ( ((x) > 0.0) ? 1.0 : (((x) < 0.0) ? -1.0 : 0.0) )

#ifdef   MAX
#undef   MAX
#endif
#define  MAX( x, y )  ( ((x) >= (y)) ? (x) : (y) )

#define  MAX3( x, y, z )  ( ((x) >= (y)) ? MAX( x, z ) : MAX( y, z ) )

#ifdef   MIN
#undef   MIN
#endif
#define  MIN( x, y )  ( ((x) <= (y)) ? (x) : (y) )

#define  MIN3( x, y, z )  ( ((x) <= (y)) ? MIN( x, z ) : MIN( y, z ) )

/* --------- gets the address of a 2-d array element in a 1-d array ----- */

#define  IJ( i, j, nj )          ( (i) * (nj) + (j) )

/* --------- gets the address of a 3-d array element in a 1-d array ----- */

#define  IJK( i, j, k, nj, nk )  ( (k) + (nk) * ((j) + (nj) * (i)) )

/* --------- environment variables -------------------------- */

#define  ENV_EXISTS( env ) ( getenv(env) != (char *) 0 )

/* --------- C and LINT stuff -------------------------- */

#ifdef __STDC__
#define GLUE(x,y) x##y
#define GLUE3(x,y,z) x##y##z
#define CREATE_STRING(x) #x
#else
#define GLUE(x,y) x/**/y
#define GLUE3(x,y,z) x/**/y/**/z
#define CREATE_STRING(x) "x"
#endif

/* Basic types */

typedef  signed char     Smallest_int;

typedef  unsigned char   unsigned_byte;

typedef  int             BOOLEAN;

typedef  double          Real;

#define  REAL_MAX        DBL_MAX

typedef  char            *STRING;

/* --------------- */

#define  IS_INT( x )    ((double) (x) == (double) ((int) (x)))

#define  FLOOR( x )     ((int) floor(x))

#define  ROUND( x )     FLOOR( (double) (x) + 0.5 )

#define  CEILING( x )   ((int) ceil(x))

#define  FRACTION( x )  ((double) (x) - (double) FLOOR(x))

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
