#ifndef  DEF_STRING
#define  DEF_STRING

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
