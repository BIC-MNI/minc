#ifndef  DEF_MATH
#define  DEF_MATH

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
