/* ----------------------------- MNI Header -----------------------------------
@NAME       : type_limits.h
@DESCRIPTION: Includes limits.h and float.h, undefining things that are
              defined in both to avoid errors from lint (on SGI).
@METHOD     : 
@CREATED    : August 7, 1992 (Peter Neelin)
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
@RCSID      : $Header: /private-cvsroot/minc/libsrc/type_limits.h,v 1.3 1993-07-20 12:18:04 neelin Exp $ MINC (MNI)
---------------------------------------------------------------------------- */

#include <limits.h>

/* Undefine those things that get redefined in float.h */
#ifdef FLT_DIG
#undef FLT_DIG
#endif
#ifdef DBL_DIG
#undef DBL_DIG
#endif
#ifdef DBL_MIN
#undef DBL_MIN
#endif
#ifdef DBL_MAX
#undef DBL_MAX
#endif

#include <float.h>
