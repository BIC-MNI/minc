/* ----------------------------- MNI Header -----------------------------------
@NAME       : vax_conversions.h
@DESCRIPTION: Header file for vax type conversion routines
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993
@MODIFIED   : 
 * $Log: vax_conversions.h,v $
 * Revision 6.2  1999-10-19 15:57:19  neelin
 * Fixed log message containing log substitution
 *
 * Revision 6.1  1999/10/19 14:45:15  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:41  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:41  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:50  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:35  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:34:32  neelin
 * Release of minc version 0.2
 *
 * Revision 1.4  94/09/28  10:34:21  neelin
 * Pre-release
 * 
 * Revision 1.3  93/08/04  13:04:03  neelin
 * Added RCS Log to keep track of modifications in source
 *
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

