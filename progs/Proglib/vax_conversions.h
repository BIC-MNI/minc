/* ----------------------------- MNI Header -----------------------------------
@NAME       : vax_conversions.h
@DESCRIPTION: Header file for vax type conversion routines
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993
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
---------------------------------------------------------------------------- */


void get_vax_short(int nvals, void *vax_value, short *mach_value);
void get_vax_long(int nvals, void *vax_value, long *mach_value);
void get_vax_float(int nvals, void *vax_value, float *mach_value);

