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
---------------------------------------------------------------------------- */

#ifndef lint
static char string_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/string_funcs.h,v 1.5 1995-07-31 13:44:32 david Exp $";
#endif

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

#define  EQUAL_STRINGS( str1, str2 )  ( strcmp(str1,str2) == 0 )

#define  IS_LOWER_CASE( ch )     ((ch) >= 'a' && (ch) <= 'z')
#define  IS_UPPER_CASE( ch )     ((ch) >= 'A' && (ch) <= 'Z')

#define  GET_LOWER_CASE( ch )  (IS_UPPER_CASE(ch) ? \
                                (char) ((ch)+'a'-'A') : (ch))

#define  GET_UPPER_CASE( ch )  (IS_LOWER_CASE(ch) ? \
                                (char) ((ch)+'A'-'a') : (ch))

#define  COPY_MEMORY( dest, src, n_items )                                  \
         (void) memcpy( (void *) (dest), (void *) (src),                    \
                        (size_t) (n_items) * sizeof(src[0]) )

#endif
