/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /private-cvsroot/minc/progs/mincgen/generic.h,v 1.1 2004-06-15 20:14:40 bert Exp $
 *********************************************************************/

#ifndef UD_GENERIC_H
#define UD_GENERIC_H

union generic {			/* used to hold any kind of fill_value */
    float floatv;
    double doublev;
    int intv;
    short shortv;
    char charv;
};

#endif
