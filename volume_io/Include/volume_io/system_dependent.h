#ifndef  _DEF_SYSTEM_DEPENDENT_H

#ifdef sun
#ifndef DBL_MAX
#include <values.h>
#define DBL_MAX  MAXDOUBLE
#endif

#define  EXIT_FAILURE  1
#define  EXIT_SUCCESS  0
#endif

#endif
