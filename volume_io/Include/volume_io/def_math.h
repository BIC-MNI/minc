#ifndef  DEF_MATH
#define  DEF_MATH

/*  Redefines y1 and y0 so lint does not make erroneous messages */

#define  y1   math_y1
#define  y0   math_y0

#include  <math.h>

#undef  y1
#undef  y0

#endif
