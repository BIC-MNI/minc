/* ----------------------------- MNI Header -----------------------------------
@NAME       : type_limits.h
@DESCRIPTION: Includes limits.h and float.h, undefining things that are
              defined in both to avoid errors from lint (on SGI).
@METHOD     : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
@RCSID      : $Header: /private-cvsroot/minc/libsrc/type_limits.h,v 1.2 1992-12-01 14:05:02 neelin Exp $ MINC (MNI)
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
