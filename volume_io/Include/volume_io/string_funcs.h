#ifndef  DEF_STRING
#define  DEF_STRING

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
@VERSION    : $Header: /private-cvsroot/minc/volume_io/Include/volume_io/string_funcs.h,v 1.7 2001-12-14 17:12:27 neelin Exp $
---------------------------------------------------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : string.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Macros for string manipulations
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <string.h>

#define  EXTREMELY_LARGE_STRING_SIZE  10000

#define  END_OF_STRING                  ((char) 0)

#define  COPY_MEMORY( dest, src, n_items )                                  \
         (void) memcpy( (void *) (dest), (void *) (src),                    \
                        (size_t) (n_items) * sizeof((src)[0]) )

#endif
