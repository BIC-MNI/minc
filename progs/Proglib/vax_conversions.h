/* ----------------------------- MNI Header -----------------------------------
@NAME       : vax_conversions.h
@DESCRIPTION: Header file for vax type conversion routines
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993
@MODIFIED   : 
---------------------------------------------------------------------------- */


void get_vax_short(int nvals, void *vax_value, short *mach_value);
void get_vax_long(int nvals, void *vax_value, long *mach_value);
void get_vax_float(int nvals, void *vax_value, float *mach_value);

