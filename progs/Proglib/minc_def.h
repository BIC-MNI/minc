/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_def.h
@DESCRIPTION: Header file to define general things needed for minc programs
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 7, 1993 (Peter Neelin)
@MODIFIED   : $Log: minc_def.h,v $
@MODIFIED   : Revision 1.4  1993-08-11 13:21:51  neelin
@MODIFIED   : Added macros ROUND and STR_EQ.
@MODIFIED   :
 * Revision 1.3  93/08/04  13:03:30  neelin
 * Added RCS $Log: minc_def.h,v $
 * Added RCS Revision 1.4  1993-08-11 13:21:51  neelin
 * Added RCS Added macros ROUND and STR_EQ.
 * Added RCS to keep track of modifications in source.
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

#define ROUND( x ) ((x) + ( ((x) >= 0) ? 0.5 : (-0.5) ) )

#define STR_EQ(s1,s2) (strcmp(s1,s2)==0)
