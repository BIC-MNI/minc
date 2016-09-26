/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_def.h
@DESCRIPTION: Header file to define general things needed for minc programs
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 7, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: minc_def.h,v $
 * Revision 6.2  1999/10/19 15:57:17  neelin
 * Fixed log message containing log substitution
 *
 * Revision 6.1  1999/10/19 14:45:13  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:41  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:41  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:50  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:35  neelin
 * Release of minc version 0.3
 *
 * Revision 2.1  1994/12/02  09:03:05  neelin
 * Added definitions of public and private.
 *
 * Revision 2.0  94/09/28  10:34:30  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.6  94/09/28  10:34:22  neelin
 * Pre-release
 * 
 * Revision 1.5  93/08/11  13:23:31  neelin
 * Modified ROUND macro to cast to (long)
 * 
 * Revision 1.4  93/08/11  13:21:51  neelin
 * Added macros ROUND and STR_EQ.
 * 
 * Revision 1.3  93/08/04  13:03:30  neelin
 * Added RCS Log to keep track of modifications in source
 *
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#  define EXIT_FAILURE 1
#endif
#ifndef SEEK_SET
#  define SEEK_SET 0
#endif

#ifdef MALLOC
#  undef MALLOC
#endif
#ifdef FREE
#  undef FREE
#endif
#ifdef REALLOC
#  undef REALLOC
#endif
#ifdef CALLOC
#  undef CALLOC
#endif

#define MALLOC(size) ((void *) malloc(size))

#define FREE(ptr) free(ptr)

#define REALLOC(ptr, size) ((void *) realloc(ptr, size))

#define CALLOC(nelem, elsize) ((void *) calloc(nelem, elsize))

/* Define useful macros */

#ifdef ROUND
#  undef ROUND
#endif

#ifdef STR_EQ
#  undef STR_EQ
#endif

#define ROUND( x ) ((long) ((x) + ( ((x) >= 0) ? 0.5 : (-0.5) ) ))

#define STR_EQ(s1,s2) (strcmp(s1,s2)==0)
