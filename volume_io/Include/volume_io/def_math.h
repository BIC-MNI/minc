#ifndef  DEF_MATH
#define  DEF_MATH

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
static char rcsid_math[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/def_math.h,v 1.7 1995-07-31 13:44:35 david Exp $";
#endif

/*  Redefines y1 and y0 so lint does not make erroneous messages */

#ifdef lint
#define  y1   math_y1
#define  y0   math_y0
#define  fcabs( x )   fcabs_avoid_lint_message( float )
#endif

#include  <math.h>

#ifdef lint
#undef  fcabs
#undef  y1
#undef  y0
#endif

#endif
