/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_def.h
@DESCRIPTION: Header file to define general things needed for minc programs
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 7, 1993 (Peter Neelin)
@MODIFIED   : 
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

#define MALLOC(size) ((void *) malloc(size))

#define FREE(ptr) free(ptr)

#define REALLOC(ptr, size) ((void *) realloc(ptr, size))

#define CALLOC(nelem, elsize) ((void *) calloc(nelem, elsize))

